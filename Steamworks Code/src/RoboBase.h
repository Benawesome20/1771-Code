/*
 * DriveTrain.h
 *
 *  Created on: January 21, 2017
 *      Author: Benjamin Ventimiglia
 */

#ifndef SRC_ROBOBASE_H_
#define SRC_ROBOBASE_H_

#include <WPILib.h>
#include <CANTalon.h>
#include <Definitions.h>
#include "Transmission.h"
#include "Pixy.h"

class RoboBase {
	Transmission l_motor;
	Transmission r_motor;
	Solenoid shift;
	frc::Joystick l_stick;
	frc::Joystick r_stick;
	Pixy gear_cam;

public:
	RoboBase(int lmotor_port, int rmotor_port, int lencoder_channel1, int lencoder_channel2, int rencoder_channel1, int rencoder_channel2, int shift_port, int lstick_port, int rstick_port):
		l_motor(lmotor_port, lencoder_channel1, lencoder_channel2),
		r_motor(rmotor_port, rencoder_channel1, rencoder_channel2),
		shift(shift_port),
		l_stick(lstick_port),
		r_stick(rstick_port),
		gear_cam(1, 3)
    {
	}

	/* Basic tank drive */
	void TankDrive() {
		l_motor.Set(-l_stick.GetY());
		r_motor.Set(r_stick.GetY());
	}

	/* Automatically shifts gears based on average speed of motors */
	void AutoShift()
	{
		if(GetSpeed() > SHIFT_UP_SPEED)
			shift.Set(1);
		else if(GetSpeed() < SHIFT_DOWN_SPEED)
			shift.Set(0);
	}

	/* Shifts the gear of the motors to bool 'gear' */
	void Shift(bool gear)
	{
		shift.Set(gear);
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
	void SetAll(double speed) {
		l_motor.Set(-speed);
		r_motor.Set(speed);
	}

	/* Gets the y axis of the left joystick */
	double GetLeftYAxis()
	{
		return l_stick.GetY();
	}

	/* Gets the y axis of the right joystick */
	double GetRightYAxis()
	{
		return r_stick.GetY();
	}

	/* Gets the button 'butt' of the left joystick */
	double GetLeftButton(int butt)
	{
		return l_stick.GetRawButton(butt);
	}

	/* Gets the button 'butt' of the right joystick */
	double GetRightButton(int butt)
	{
		return r_stick.GetRawButton(butt);
	}

	/* Purpose: Turns the robot to face a target
	 *
	 * Method : Checks if the PixyCam sees a target,
	 * 			feeds the offset of the target into the motors,
	 * 			and stops if the offset is within (-0.05, 0.05) */
	void TrackHook()
	{
		if ((int)gear_cam.GetOffset() != -2 && (gear_cam.GetOffset() > 0.05 && gear_cam.GetOffset() < -0.05))
		{
			l_motor.Set(gear_cam.GetOffset());
			r_motor.Set(gear_cam.GetOffset());
		}
		else
		{
			StopMotors();
		}
	}

	/* Finds an average distance between the motors in case of discrepancies */
	double GetDistance()
	{
		return fabs(l_motor.GetDistance() + r_motor.GetDistance()) / 2;
	}

	/* Returns the average speed of the motors */
	double GetSpeed()
	{
		return fabs((l_motor.GetSpeed())-r_motor.GetSpeed()) / 2;
	}

	/* Purpose: Drives in 'dir' direction at 'speed' speed for 'distance' distance
	 *
	 * Method : Finds the sign of the direction,
	 * 			gets the average distance in the direction of 'sign'
	 * 			checks if the current position is less than the target distance
	 * 			checks for discrepancies within 0.1 in left and right encoder distances, indicating the robot is turning
	 * 			sets the motors to turn the opposite way
	 * 			if no discrepancies between right and left, set both motors to the same speed */
	void DriveXDistance(int distance, int speed, int dir) {
		int sign = (r_motor.GetDistance()+l_motor.GetDistance())/fabs(r_motor.GetDistance()+l_motor.GetDistance());
		double pos = sign * GetDistance();
		if((pos < distance))
		{
				if(fabs(r_motor.GetDistance()) > fabs(l_motor.GetDistance()) + 0.1)
				{
						r_motor.Set(speed * .75);
						l_motor.Set(-1);
				}
				else if(fabs(r_motor.GetDistance()) + 0.1 < fabs(l_motor.GetDistance()))
				{
						r_motor.Set(1);
						l_motor.Set(speed * -.75);
				}
				else
				{
						r_motor.Set(speed);
						l_motor.Set(-speed);
				}
		}
		else
		{
				StopMotors();
		}
	}
};

#endif /* SRC_ROBOBASE_H_ */