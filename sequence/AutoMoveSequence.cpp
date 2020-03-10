#include "AutoMoveSequence.hpp"

using namespace eeduro::delta;

AutoMoveSequence::AutoMoveSequence(std::string name, Sequence* caller, DeltaControlSystem& controlSys, SafetySystem& safetySys, DeltaSafetyProperties& safetyProp, Calibration& calibration):
	Sequence(name, caller, true),
	sortSeq("Sort Sequence", this, controlSys, calibration),
	shuffSeq("Shuffle Sequence", this, controlSys, calibration),
	wait("wait", this),
	moveMouseCondition(controlSys),
	mouseExceptionSeq("Mouse Exception Sequence", this,  safetySys, safetyProp),
	moveMouseMonitor("MouseMoveMonitor", this, moveMouseCondition, SequenceProp::abort, &mouseExceptionSeq),
	safetySys(safetySys),
	safetyProp(safetyProp),
	controlSys(controlSys),
	blueButtonCondition(),
	blueButtonExceptionSeq("Blue button exception sequence", this, controlSys, safetySys, safetyProp),
	blueButtonMonitor("BlueButtonMonitor", this, blueButtonCondition, SequenceProp::abort, &blueButtonExceptionSeq) { 
		addMonitor(&moveMouseMonitor);
		addMonitor(&blueButtonMonitor);
	}
	


int AutoMoveSequence::action() {
	moveMouseCondition.reset();
	controlSys.setPathPlannerInput();
	while(getRunningState() == SequenceState::running) {
		auto res = sortSeq.start();
		moveMouseCondition.reset();
		wait(5);
		if (res == 0) shuffSeq.start();
		wait(5);
	}
}


