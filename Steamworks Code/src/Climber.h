/*
 * Climber.h
 *
 *  Created on: Mar 9, 2017
 *      Author: Justin Desimpliciis
 */

#ifndef SRC_CLIMBER_H_
#define SRC_CLIMBER_H_

#include <WPILib.h>
#include <CANTalon.h>

#include "Definitions.h"
#include "Transmission.h"

class Climber {
	Transmission climb_motor;
	double startingDist;

public:
	Climber(int climb_port1, int climb_port2):
		climb_motor(climb_port1, climb_port2 , 0, 0)
	{
		startingDist = 0;
		climb_motor.ConfigNeutralMode(CANTalon::kNeutralMode_Brake);
	}

	void ClimbUp(int current)
	{
		if (climb_motor.GetAvgOutputCurrent() < current)
		{
			climb_motor.Set(1);
		}
		else
		{
			climb_motor.Set(0);
		}

		//^^ I added the else statement, but I'm not sure if it's necessary // it is
	}

	void ManualClimb(double rate)
	{
		climb_motor.Set(rate);
	}

	/*double GetEncPosition()
	{
		return climb_motor.GetEncPosition();
	}*/

	double GetAvgOutputCurrent()
	{
		return climb_motor.GetAvgOutputCurrent();
	}

	double Get1OutputCurrent()
	{
		return climb_motor.Get1OutputCurrent();
	}

	double Get2OutputCurrent()
	{
		return climb_motor.Get2OutputCurrent();
	}

	/*void ClearEncoder()
	{
		climb_motor.SetEncPosition(0);
	}*/
};


#endif /* SRC_CLIMBER_H_ */
