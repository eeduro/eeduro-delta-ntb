#include <iostream>
#include <ostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <eeros/hal/HAL.hpp>
#include <eeros/core/Executor.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/sequencer/Sequencer.hpp>

#include "control/DeltaControlSystem.hpp"
#include "safety/DeltaSafetyProperties.hpp"
#include "sequence/MainSequence.hpp"

using namespace eeros;
using namespace eeros::hal;
using namespace eeros::control;
using namespace eeros::safety;
using namespace eeros::logger;
using namespace eeros::sequencer;
using namespace eeduro::delta;

void signalHandler(int signum){
	SafetySystem::exitHandler();
	Sequencer::instance().abort();
}

int main(int argc, char **argv) {

	StreamLogWriter w(std::cout);
	Logger::setDefaultWriter(&w);
	Logger log;
// 	w.show();
	log.info() << "Start Delta application";
	
	log.info() << "Initializing Hardware...";
	HAL& hal = HAL::instance();
	hal.readConfigFromFile(&argc, argv);
	
	signal(SIGINT, signalHandler);
	
	// Create the control system
	DeltaControlSystem controlSys;
	
	// Create and initialize a safety system
	DeltaSafetyProperties safetyProp{controlSys};
	SafetySystem safetySys{safetyProp, dt};
	controlSys.timedomain.registerSafetyEvent(safetySys, safetyProp.doEmergency);
	
	Calibration calibration{};
	calibration.loadDefaults();
	if (!calibration.load()) {
		log.warn() << "could not load calibration";
	}
	
	auto& seq = Sequencer::instance();
	MainSequence mainSequence("Main Sequence", seq, controlSys, safetySys, safetyProp, calibration);
	seq.addSequence(mainSequence);
	mainSequence.start();
	
	auto &executor = Executor::instance();
	executor.setMainTask(safetySys);
	executor.run();
	
	seq.wait();
	
	log.info() << "Delta application finished";
	return 0;
}