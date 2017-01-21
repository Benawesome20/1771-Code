/*
 * DriveTrain.h
 *
 *  Created on: January 21, 2017
 *      Author: Benjamin Ventimiglia
 */

#ifndef SRC_DRIVETRAIN_H_
#define SRC_DRIVETRAIN_H_

#include <iostream>
#include <memory>
#include <string>

#include <WPILib.h>
#include <CANTalon.h>
#include "Transmission.h"

class DriveTrain {
	/* Objects for class use */
	Transmission l_motor;
	Transmission r_motor;
	frc::Joystick l_stick;
	frc::Joystick r_stick;

public:
	/* Initialize the class objects */
	DriveTrain(int lmotor_port, int rmotor_port, int lencoder_channel1, int lencoder_channel2, int rencoder_channel1, int rencoder_channel2, int lstick_port, int rstick_port):
		l_motor(lmotor_port, lencoder_channel1, lencoder_channel2),
		r_motor(rmotor_port, rencoder_channel1, rencoder_channel2),
		l_stick(lstick_port),
		r_stick(rstick_port)
    {
	}

	/* Basic tank drive */
	void TankDrive() {
		l_motor.Set(-l_stick.GetY());
		r_motor.Set(r_stick.GetY());
	}

	/* Resets all encoder values */
	void Reset() {
		l_motor.Reset();
		r_motor.Reset();
	}

	/* Stops all motor movement */
	void StopMotors() {
		l_motor.Set(0);
		r_motor.Set(0);
	}

	/* Sets all the motors at once */
	void SetAll(int speed) {
		l_motor.Set(-speed);
		r_motor.Set(speed);
	}

	/* Finds an average distance between the motors in case of discrepancies */
	double GetDistance() {
		return fabs(l_motor.GetDistance() + r_motor.GetDistance()) / 2;
	}

	/* Drives in 'dir' direction at 'speed' speed for 'distance' distance */
	void DriveXDistance(int distance, int speed, int dir) {
		while (GetDistance() < distance) {
			SetAll(speed * (dir/fabs(dir)));
		}
	}
};


#endif /* SRC_DRIVETRAIN_H_ */