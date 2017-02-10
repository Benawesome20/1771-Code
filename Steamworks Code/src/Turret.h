/*
 * Turret.h
 *
 *  Created on: January 21, 2017
 *      Author: Benjamin Ventimiglia
 */

#ifndef SRC_TURRET_H_
#define SRC_TURRET_H_

#include <WPILib.h>
#include <CANTalon.h>
#include "Transmission.h"
#include "Pixy.h"

class Turret {
	Transmission t_motor;
	Transmission aim_motor;
	Transmission shoot_motor;
public:
	Turret()
	{
	}

	void Aim()
	{
	}

	double GetRotation()
	{
		return t_motor.GetDistance() % 360; //TEST PLS
	}

	double GetHeight()
	{
		return 0;
	}

	void SetRotation()
	{
	}

	void SetHeight()
	{
	}
};

#endif /* SRC_TURRET_H_ */
