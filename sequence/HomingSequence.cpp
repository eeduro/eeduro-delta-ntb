#include "HomingSequence.hpp"
#include "../control/types.hpp"

using namespace eeduro::delta;

HomingSequence::HomingSequence(std::string name, Sequence* caller, DeltaControlSystem& controlSys, SafetySystem& safetySys, DeltaSafetyProperties& safetyProp, Calibration& calibration): 
	Sequence(name, caller, true),
	controlSys(controlSys),
	safetyProp(safetyProp),
	safetySys(safetySys),
	wait("wait", this),
	move("move", this, controlSys),
	calibration(calibration)
	{ }

int HomingSequence::action() {
	AxisVector torqueLimit{ q012gearTorqueLimit, q012gearTorqueLimit, q012gearTorqueLimit, q3gearTorqueLimit };
	controlSys.torqueLimitation.setLimit(-torqueLimit, torqueLimit);
	AxisVector forceLimit{ 100, 100, 100, 100 };
	controlSys.forceLimitation.setLimit(-forceLimit, forceLimit);
	AxisVector accLimit{ 100, 100, 100, 100 };
	controlSys.accLimitation.setLimit(-accLimit, accLimit);
	controlSys.voltageSetPoint.setValue({q012InitVoltage, q012InitVoltage, q012InitVoltage, q3InitVoltage});
	controlSys.voltageSwitch.switchToInput(1);	// choose fix voltage setpoint
	wait(2.5);
	controlSys.enc1.callInputFeature<>("setFqdPos", q012homingOffset);
	controlSys.enc2.callInputFeature<>("setFqdPos", q012homingOffset);
	controlSys.enc3.callInputFeature<>("setFqdPos", q012homingOffset);
	controlSys.enc4.callInputFeature<>("setFqdPos", q3homingOffset);
	wait(0.1);
	controlSys.pathPlanner.setInitPos(controlSys.directKin.getOut().getSignal().getValue());
	controlSys.setPathPlannerInput();
	wait(0.1);
	controlSys.voltageSwitch.switchToInput(0);	// choose controller setpoint
	move({0, 0, tcpReady_z, 0});	
	safetySys.triggerEvent(safetyProp.homingDone);
}
