#pragma once

#include <vector>

#include <eeros/sequencer/Sequence.hpp>
#include <eeros/sequencer/Sequencer.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/sequencer/Wait.hpp>

#include "../control/DeltaControlSystem.hpp"
#include "../safety/DeltaSafetyProperties.hpp"
#include "../Calibration.hpp"
#include "step/Move.hpp"
#include "step/WaitForLevel.hpp"

using namespace eeros::sequencer;
using namespace eeros::safety;

namespace eeduro {
	namespace delta {
		class MouseExceptionSequence : public Sequence{
			public:
				MouseExceptionSequence(std::string name, Sequence* caller, SafetySystem& safetySys, DeltaSafetyProperties& safetyProp):
				Sequence(name, caller, true),
				safetySys(safetySys),
				safetyProp(safetyProp) { }
				
				int action(){
					safetySys.triggerEvent(safetyProp.doMouseControl);
				}

			private:
				SafetySystem& safetySys;
				DeltaSafetyProperties& safetyProp;
		};
			
		class MouseTimeOutExceptionSequence : public Sequence{
			public:
				MouseTimeOutExceptionSequence(std::string name, Sequence* caller, SafetySystem& safetySys, DeltaSafetyProperties& safetyProp):
				Sequence(name, caller, true),
				safetySys(safetySys),
				safetyProp(safetyProp) { }
				
				int action(){
					safetySys.triggerEvent(safetyProp.doAutoMoving);
				}
				
			private:
				SafetySystem& safetySys;
				DeltaSafetyProperties& safetyProp;
		};
		
		class BlueButtonExceptionSequence : public Sequence{
			public:
				BlueButtonExceptionSequence(std::string name, Sequence* caller, DeltaControlSystem& controlSys, SafetySystem& safetySys, DeltaSafetyProperties& safetyProp):
				Sequence(name, caller, true),
				safetySys(safetySys),
				safetyProp(safetyProp),
				calibration(calibration),
				move("Move", this, controlSys),
				waitForLevel("WaitForLevel", this, safetySys),
				controlSys(controlSys),
				wait("Wait", this){}
				
				int action(){
					eeros::math::Vector<4> torqueLimit{ q012gearTorqueLimit, q012gearTorqueLimit, q012gearTorqueLimit, q3gearTorqueLimit };
					controlSys.torqueLimitation.setLimit(-torqueLimit, torqueLimit);
					controlSys.setPathPlannerInput();
					AxisVector p = controlSys.directKin.getOut().getSignal().getValue();
					controlSys.pathPlanner.setInitPos(p);
					
					log.warn() << p;
					p[2] = calibration.transportation_height;
					log.warn() << p;
					move(p);
					p = { 0, 0, calibration.transportation_height, 0 };
					move(p);
					safetySys.triggerEvent(safetyProp.stopMoving);
					waitForLevel(safetyProp.slSystemReady);
				}
				
			private:
				SafetySystem& safetySys;
				DeltaSafetyProperties& safetyProp;
				DeltaControlSystem& controlSys;
				Wait wait;
				
				Move move;
				Calibration& calibration;
				WaitForLevel waitForLevel;
		};
		
		class EmergencyExceptionSequence : public Sequence {
		public:
			EmergencyExceptionSequence(std::string name, Sequence* caller, DeltaControlSystem& controlSys, SafetySystem& safetySys, DeltaSafetyProperties& safetyProp) :
				Sequence(name, caller, true),
				safetySys(safetySys),
				safetyProp(safetyProp),
				controlSys(controlSys) { }
				
			int action(){
				controlSys.voltageSetPoint.setValue({0, 0, 0, 0});
				controlSys.voltageSwitch.switchToInput(1);
				HAL::instance().getLogicOutput("ledBlue", false)->set(false);
				while (!HAL::instance().getLogicInput("buttonGreen", false)->get());
				safetySys.triggerEvent(safetyProp.doControlStart);
			}

		private:
			SafetySystem& safetySys;
			DeltaSafetyProperties& safetyProp;
			DeltaControlSystem& controlSys;
		};
	}
}
