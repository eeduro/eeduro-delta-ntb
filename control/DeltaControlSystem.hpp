#pragma once

#include <eeros/core/Executor.hpp>
#include <eeros/logger/Logger.hpp>
#include <eeros/control/Sum.hpp>
#include <eeros/control/D.hpp>
#include <eeros/control/Gain.hpp>
#include <eeros/control/Constant.hpp>
#include <eeros/control/PeripheralInput.hpp>
#include <eeros/control/PeripheralOutput.hpp>
#include <eeros/control/TimeDomain.hpp>
#include <eeros/control/MouseInput.hpp>
#include <eeros/control/Switch.hpp>
#include <eeros/control/Mux.hpp>
#include <eeros/control/DeMux.hpp>
#include <eeros/control/Saturation.hpp>

#include "types.hpp"
#include "Kinematic.hpp"
#include "Jacobian.hpp"
#include "DirectKinematic.hpp"
#include "Jacobi.hpp"
#include "Inertia.hpp"
#include "MotorModel.hpp"
#include "constants.hpp"
#include "PathPlanner.hpp"

using namespace eeros::control;
using namespace eeros::logger;

namespace eeduro{
	namespace delta{
		class DeltaControlSystem {
			public:
				DeltaControlSystem();
				~DeltaControlSystem();

				void start();
				void stop();

				/*
				* ###
				* Methods for the sequencer
				* ###
				*/
				void setMouseInput();
				void setPathPlannerInput();

				/*
				* ###
				* Components of the Controlsystem
				* ###
				*/
				AxisVector kM;
				AxisVector RA;
				Kinematic kinematic;
				Jacobian jacobian;

				MouseInput mouse;
				PathPlanner pathPlanner;

				Switch<2,AxisVector> posSwitch;

				PeripheralInput<double> enc1;
				PeripheralInput<double> enc2;
				PeripheralInput<double> enc3;
				PeripheralInput<double> enc4;

				PeripheralOutput<double> mot1;
				PeripheralOutput<double> mot2;
				PeripheralOutput<double> mot3;
				PeripheralOutput<double> mot4;

				Mux<4, double, AxisVector> muxEnc;
				DeMux<4, double, AxisVector> demuxMot;

				eeros::control::D<AxisVector> posDiff;

				Sum<2, AxisVector> posSum;
				Gain<AxisVector> posController;

				Sum<3, AxisVector> speedSum;
				Saturation<AxisVector> speedLimitation;
				Gain<AxisVector> speedController;

				Sum<2,AxisVector> accSum;
				Saturation<AxisVector> accLimitation;

				Inertia inertia;
				Saturation<AxisVector> forceLimitation;
				Constant<AxisVector> forceSetPoint;
				Sum<2, AxisVector> forceSum;
				Jacobi jacobi;
				Saturation<AxisVector> torqueLimitation;

				MotorModel motorModel;

				Switch<2, AxisVector> voltageSwitch;
				Constant<AxisVector> voltageSetPoint;

				PeripheralOutput<bool> emag;
				Constant<bool> emagVal;  

				DirectKinematic directKin;
				
				Constant<AxisVector> accSetPoint;
				Constant<AxisVector> velSetPoint;
				
				Switch<2, AxisVector> accSwitch;
				Switch<2, AxisVector> velSwitch;
				
				TimeDomain timedomain;
				Logger log;
		};
	} 
}