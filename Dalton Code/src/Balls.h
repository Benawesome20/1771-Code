/*
 * Balls.h
 *
 *  Created on: January 21, 2017
 *      Author: Benjamin Ventimiglia
 */

#ifndef SRC_BALLS_H_
#define SRC_BALLS_H_

#include <WPILib.h>
#include <CANTalon.h>
#include "Turret.h"
#include "Intake.h"

class Balls {
	/* Too lazy to create get functions for both the turret and intake,
	 * so I made them public so I don't have to deal with them */
public:
	Turret turret;
	Intake intake;
	Climber c_motor;
	frc::Joystick t_stick;

	Balls(int tmotor_port, int amotor_port, int smotor_port, int imotor_port, int c_motor_port, int tencoder_1,
		  int tencoder_2, int aencoder_1, int aencoder_2, int tstick_port):
		turret(tmotor_port, amotor_port, smotor_port, tencoder_1, tencoder_2, aencoder_1, aencoder_2),
		intake(imotor_port),
		c_motor(c_motor_port),
		t_stick(tstick_port)
	{
	}

	void SetIntake(double rate)
	{
		intake.Set(rate);
	}

	bool GetTurretButton(int butt)
	{
		return t_stick.GetRawButton(butt);
	}

	double GetTurretYAxis()
	{
		return t_stick.GetY();
	}

	double GetTurretXAxis()
	{
		return t_stick.GetX();
	}

	void TurretStickDrive()
	{
		if(!((turret.GetRealRotation() > 30) || (turret.GetRealRotation() < -30)))
			turret.SetRotation(t_stick.GetX());
		else
			turret.StopAll();

		if(!((turret.GetHeight() > 20) || (turret.GetHeight() < -5)))
			turret.SetHeight(t_stick.GetY());
		else
			turret.StopAll();
	}

	void ClimpUp(int distance)
	{
		c_motor.ClimbUp(distance);
	}

	void ManualClimb()
	{
		c_motor.ManualClimb(t_stick.GetY()); // figure out actual stick to use
	}

	double GetClimbEncoder()
	{
		return c_motor.GetEncPosition();
	}

	void ClearClimbEncoder()
	{
		c_motor.ClearEncoder();
	}

	void StopTurretFire()
	{
		turret.StopFire();
	}

};
#endif /* SRC_BALLS_H_ */
