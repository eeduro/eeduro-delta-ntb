#include "MainSequence.hpp"

using namespace eeduro::delta;

MainSequence::MainSequence(std::string name, Sequencer& sequencer, DeltaControlSystem& controlSys, SafetySystem& safetySys, DeltaSafetyProperties safetyProp, Calibration& calibration):
	Sequence(name, sequencer),
	log('M'),
	safetyProp(safetyProp),
	safetySys(safetySys),
	controlSys(controlSys),
	homingSeq("Homing sequence", this, controlSys, safetySys, safetyProp, calibration),
	automoveSeq("AutoMove sequence", this, controlSys, safetySys, safetyProp, calibration),
	parkSeq("Park sequence", this, controlSys, safetySys, safetyProp),
	calibrateSeq("Calibrate sequence", this, controlSys, calibration),
	mouseSeq("Mouse sequence", this, controlSys, safetySys, safetyProp),
	wait("Wait in main", this),
	waitForLevel("Wait for level", this, safetySys),
	emergencyCondition(safetySys, safetyProp),
	emergencyExceptionSeq("Emergency exception sequence", this, controlSys, safetySys, safetyProp), 
	emergencyMonitor("Emergency level monitor", this, emergencyCondition, eeros::sequencer::SequenceProp::restart, &emergencyExceptionSeq) { 
		HAL& hal = HAL::instance();
		buttonGreen = hal.getLogicInput("buttonGreen", false);
		buttonBlue = hal.getLogicInput("buttonBlue", false);
		ledBlue = hal.getLogicOutput("ledBlue", false);
		addMonitor(&emergencyMonitor);
	}
	
int MainSequence::action() {
	ledBlue->set(false);
	waitForLevel(safetyProp.slSystemOn);
	safetySys.triggerEvent(safetyProp.doPoweringUp);
	waitForLevel(safetyProp.slPoweringUp);
	safetySys.triggerEvent(safetyProp.doHoming);
	waitForLevel(safetyProp.slHoming);
	homingSeq.start();
	waitForLevel(safetyProp.slHomed);
	safetySys.triggerEvent(safetyProp.doSystemReady);
	waitForLevel(safetyProp.slSystemReady);
	ledBlue->set(true);
	auto bluePrev = buttonBlue->get();

	while (getRunningState() == SequenceState::running) {
		
		if(safetySys.getCurrentLevel() == safetyProp.slSystemReady) {
			
			if (buttonGreen->get()) {
				blueButtonCounter = 0;
				if(buttonBlue->get()) calibrateSeq.start();
				else safetySys.triggerEvent(safetyProp.doAutoMoving);     // go to auto moving
			}
			auto blue = buttonBlue->get();
			if (blue && blue != bluePrev) {
				blueButtonCounter++;
				if (blueButtonCounter >= 3) {                    // doParking and stop the application
					ledBlue->set(false);
					safetySys.triggerEvent(safetyProp.doParking);
				}
			}
			bluePrev = blue;
		} else if (safetySys.getCurrentLevel() == safetyProp.slAutoMoving) {
			automoveSeq.start();
		} else if (safetySys.getCurrentLevel() == safetyProp.slMouseControl) {
			mouseSeq.start();
		} else if (safetySys.getCurrentLevel() == safetyProp.slParking) {
			parkSeq.start();
		} else if (safetySys.getCurrentLevel() == safetyProp.slParked) {
			safetySys.triggerEvent(safetyProp.doControlStop);
		}
		
		wait(0.2);
	}

}

