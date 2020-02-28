#include "DetectSequence.hpp"

using namespace eeduro::delta;

DetectSequence::DetectSequence(std::string name, Sequence* caller, DeltaControlSystem& controlSys, Calibration& calibration): 
	Sequence(name, caller, true),
	controlSys(controlSys),
	calibration(calibration),
	move("move", this, controlSys),
	wait("wait", this) { }

int DetectSequence::operator()(int pos) {
	this-> position = pos;
	return start();
}

int DetectSequence::action() {	
	eeros::math::Vector<4> torqueLimit{ q012gearTorqueLimit, q012gearTorqueLimit, q012gearTorqueLimit, q3gearTorqueLimit };
	auto p = controlSys.pathPlanner.getLastPoint();
	p[3] = 0;
	double last_z = p[2];
// 	p[2] = calibration.position[position].level12 + 0.005;
// 	move(p);

	controlSys.torqueLimitation.setLimit(-torqueLimit * 0.01, torqueLimit * 0.01);
	p[2] = calibration.position[position].level30 - 0.002;
	move(p);

// 	controlSys.torqueLimitation.setLimit(-torqueLimit * 0.01, torqueLimit * 0.01);
	wait(2);		// final position may not be reached
	
	double z = controlSys.directKin.getOut().getSignal().getValue()[2];
// 	controlSys.torqueLimitation.setLimit(-torqueLimit * 0.1, torqueLimit * 0.1);
// 	p[2] = calibration.position[position].level12 + 0.002;
// 	move(p);		// start slowly move in z+

	p[2] = last_z;
	move(p);
	controlSys.torqueLimitation.setLimit(-torqueLimit, torqueLimit);

	int block = calibration.getBlock(position, z);
	log.warn() << "[DETECT] pos " << position << ": z = " << z << " -> block = " << block;

	return block;
}

