#include "MouseSequence.hpp"
#include "../safety/DeltaSafetyProperties.hpp"
#include "ExceptionSequence.hpp"

using namespace eeduro::delta;

MouseSequence::MouseSequence(std::string name, Sequence* caller, DeltaControlSystem& controlSys, SafetySystem& safetySys, DeltaSafetyProperties& safetyProp) :
	Sequence(name, caller, true),
	controlSys(controlSys),
	wait("wait in mouse move sequence", this),
	grab("grab", this, controlSys),
	release("release", this, controlSys),
	mouseTimeoutSequence("Mouse TimeOut Exception Sequence", this, safetySys, safetyProp),
	blueButtonCondition(),
	blueButtonExceptionSequence("Blue button exception sequence", this, controlSys, safetySys, safetyProp),
	blueButtonMonitor("BlueButtonMonitor", this, blueButtonCondition, SequenceProp::abort, &blueButtonExceptionSequence) {
		setTimeoutTime(2.0);
		setTimeoutExceptionSequence(mouseTimeoutSequence);
		setTimeoutBehavior(SequenceProp::abort);
		addMonitor(&blueButtonMonitor);
	}

int MouseSequence::action() {
	controlSys.setMouseInput();
	auto pos = controlSys.pathPlanner.getPosOut().getSignal().getValue();
	controlSys.mouse.setInitPos(pos[0], pos[1], pos[2], 0);
	mousePosPrev = controlSys.mouse.getOut().getSignal().getValue();

	while (getRunningState() == SequenceState::running) {
		auto pos = controlSys.mouse.getOut().getSignal().getValue();
		auto button = controlSys.mouse.getButtonOut().getSignal().getValue();
		if (pos != mousePosPrev || button != mouseButtonPrev) resetTimeout();
		if (button[0] == 1) grab(); else release();
		mousePosPrev = pos;
		mouseButtonPrev = button;
		wait(0.1);
	}
}

