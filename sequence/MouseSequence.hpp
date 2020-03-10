#pragma once

#include <array>

#include <eeros/sequencer/Sequence.hpp>
#include <eeros/safety/SafetySystem.hpp>

#include "../control/DeltaControlSystem.hpp"
#include "step/Grab.hpp"
#include "step/Release.hpp"
#include "ExceptionSequence.hpp"
#include "conditions/BlueButtonCondition.hpp"
#include "conditions/EmergencyCondition.hpp"

using namespace eeros::sequencer;
using namespace eeros::safety;

namespace eeduro {
	namespace delta {
		class MouseSequence : public Sequence {
			public:
				MouseSequence(std::string name, Sequence* caller, DeltaControlSystem& controlSys, SafetySystem& safetySys, DeltaSafetyProperties& safetyProp);
				int action();

			private:
				DeltaControlSystem& controlSys;
				AxisVector mousePosPrev;
				Matrix<3,1,bool> mouseButtonPrev;
				Wait wait;
				Grab grab;
				Release release;
				MouseTimeOutExceptionSequence mouseTimeoutSequence;
				BlueButtonExceptionSequence blueButtonExceptionSequence;
				BlueButtonCondition blueButtonCondition;
				Monitor blueButtonMonitor;
		};
	}
}
