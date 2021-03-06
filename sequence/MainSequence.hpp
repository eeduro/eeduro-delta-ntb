#pragma once

#include <array>

#include <eeros/sequencer/Sequence.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/sequencer/Monitor.hpp>
#include <eeros/sequencer/Wait.hpp>

#include "../control/DeltaControlSystem.hpp"
#include "../safety/DeltaSafetyProperties.hpp"
#include "CalibrateSequence.hpp"
#include "AutoMoveSequence.hpp"
#include "HomingSequence.hpp"
#include "ParkSequence.hpp"
#include "MouseSequence.hpp"
#include "step/WaitForLevel.hpp"

using namespace eeros::sequencer;
using namespace eeros::safety;

namespace eeduro {
	namespace delta {
		class MainSequence : public Sequence {
			public:
				MainSequence(std::string name, Sequencer& sequencer, DeltaControlSystem& controlSys, SafetySystem& safetySys, DeltaSafetyProperties safetyProp, Calibration& calibration);
				int action();
				
			private:
				HomingSequence homingSeq;
				AutoMoveSequence automoveSeq;
				ParkSequence parkSeq;
				CalibrateSequence calibrateSeq;
				MouseSequence mouseSeq;	
				Wait wait;
				WaitForLevel waitForLevel;
				
				EmergencyExceptionSequence emergencyExceptionSeq;
				EmergencyCondition emergencyCondition;
				Monitor emergencyMonitor;
				
				eeros::hal::Input<bool>* buttonGreen;
				eeros::hal::Input<bool>* buttonBlue;
				eeros::hal::Output<bool>* ledBlue;
				DeltaControlSystem& controlSys;
				SafetySystem& safetySys;
				DeltaSafetyProperties& safetyProp;
				uint8_t blueButtonCounter = 0;
				Logger log;
			};
	}
}

