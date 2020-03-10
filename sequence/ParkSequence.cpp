#include "ParkSequence.hpp"

using namespace eeduro::delta;

ParkSequence::ParkSequence(std::string name, Sequence* caller, DeltaControlSystem& controlSys, SafetySystem& safetySys, DeltaSafetyProperties& safetyProp): 
	Sequence(name, caller, true),
	controlSys(controlSys),
	safetyProp(safetyProp),
	safetySys(safetySys),
	move("park move",this, controlSys),
	wait("wait", this)
	{ }

int ParkSequence::action() {
	move({0, 0, -0.043, 0});
	wait(1);
	controlSys.voltageSetPoint.setValue({0, 0, 0, 0});	
	controlSys.voltageSwitch.switchToInput(1);
	safetySys.triggerEvent(safetyProp.parkingDone);
}
