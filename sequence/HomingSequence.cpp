#include "HomingSequence.hpp"

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
	controlSys.setVoltageForInitializing({q012InitVoltage, q012InitVoltage, q012InitVoltage, q3InitVoltage});	// choose fixed voltage
	controlSys.voltageSwitch.switchToInput(1);
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
	move({0, 0, 0, 0});
	wait(5);
//  	move({-0.013, 0.021, -0.04, 0});
// 	wait(5);
//  	move({-0.013, 0.021, -0.05, 0});
// 	wait(5);
//  	move({-0.013, 0.021, -0.04, 0});
// 	wait(5);
//  	move({0.0282, 0.0211, -0.04, 0});
// 	wait(5);
//  	move({0.0282, 0.0211, -0.05, 0});
// 	wait(5);
//  	move({0.0282, 0.0211, -0.04, 0});
// 	wait(5);
// 	wait(5);
//  	move({-0.013, 0.021, -0.04, 0});
// 	wait(5);
//  	move({-0.013, 0.021, -0.04, 0});
// 	wait(5);
//  	move({-0.013, 0.021, -0.04, 0});
//  	move({0, 0, -0.02, 0});
//  	wait(5);
	
	safetySys.triggerEvent(safetyProp.homingDone);
}
