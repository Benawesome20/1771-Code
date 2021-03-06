/*
 * RoboBase.h
 *
 *  Created on: January 21, 2017
 *      Author: Benjamin Ventimiglia
 */

#ifndef SRC_ROBOBASE_H_
#define SRC_ROBOBASE_H_

#include <WPILib.h>
#include <CANTalon.h>
#include "Climber.h"
#include "Definitions.h"
#include "Transmission.h"
#include "Pixy.h"
#include "PixyI2C.h"
#include "NavX.h"
#include <time.h>

class RoboBase {
	Transmission l_motor;
	Transmission r_motor;
	Solenoid shift;
	frc::BuiltInAccelerometer acc;
	Solenoid gear_catch;
	frc::Joystick l_stick;
	frc::Joystick r_stick;
	Pixy gear_cam;
	PixyI2C boiler_cam;
	NavX navx;
	Timer t;

	bool reverse;
	int reversedone = 50;
	double distanceTravelled = 0.0;

	clock_t lastTime;

	double setDist = 0;

public:
	RoboBase(int lmotor_port,int lmotor_port2, int rmotor_port, int rmotor_port2,
			 int lencoder_1, int lencoder_2, int rencoder_1, int rencoder_2,
			 int shift_port, int gear_port, int lstick_port, int rstick_port, int offset_port):
		l_motor(lmotor_port, lmotor_port2, lencoder_1, lencoder_2),
		r_motor(rmotor_port, rmotor_port2, rencoder_1, rencoder_2),
		shift(shift_port),
		gear_catch(gear_port),
		l_stick(lstick_port),
		r_stick(rstick_port),
		gear_cam(offset_port),
		boiler_cam(),
		acc(),
		t()
    {
	}

	void StartTimer(){
		t.Start();
	}

	void StopTimer(){
		t.Stop();
	}

	void ResetTimer(){
		t.Reset();
	}

	double GetSeconds(){
		return (double)(t.Get()*1000.0);
	}

	/* Returns offset of camera target from center */
	double GetOffSet()
	{
		return gear_cam.GetOffset();
	}

	double GetTurretXOffset(){
		return boiler_cam.getXOffset();
	}

	double GetTurretYOffset(){
		return boiler_cam.getYOffset();
	}

	/* Basic tank drive */
	void TankDrive() {
		/*l_motor.Set(GetLeftYAxis());
		r_motor.Set(-GetRightYAxis());*/

		double speed = GetLeftYAxis();

		double l_ratio = 1;
		double r_ratio = 1 - l_stick.GetX();

		if(l_stick.GetX() < 0)
		{
			double r_ratio = 1;
			double l_ratio = 1 - -l_stick.GetX();
		}

		l_motor.Set(speed * l_ratio);
		r_motor.Set(-speed * r_ratio);
	}

	double GetX()
	{
		return l_stick.GetX();
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

	/* Returns the position of the solenoid for shifting, essentially returns the gear */
	bool GetShift()
	{
		return shift.Get();
	}

	void SetCatch(bool open)
	{
		gear_catch.Set(open);
	}

	bool GetCatch()
	{
		return gear_catch.Get();
	}

	double accX = 0,
		   velX = 0,
		   posX = 0;
	void AddXAccelerometerDistance(){
		double secs = GetTime() - lastTime;
		SmartDashboard::PutString("DB/String 6", "UNIX TIME: " + std::to_string(clock()));

		accX = acc.GetX() * 9.81;
		velX += accX * secs;
		posX += velX * secs;


		lastTime = GetTime();
	}

	double accZ = 0,
		   velZ = 0,
	       posZ = 0;
	void AddZAccelerometerDistance(){
			double secs = GetTime() - lastTime;

			accZ = acc.GetZ() * 9.81;
			velZ += accZ * secs;
			posZ += velZ * secs;

			lastTime = GetTime();
	}

	double accY = 0,
		   velY = 0,
	       posY = 0;
	void AddYAccelerometerDistance(){
			double secs = GetTime() - lastTime;

			accY = acc.GetY() * 9.81;
			velY += accY * secs;
			posY += velY * secs;

			lastTime = GetTime();
	}

	double GetAccelerometerX(){
		return acc.GetX();
	}

	double GetAccelerometerZ(){
		return acc.GetZ();
	}

	double GetAccelerometerY(){
		return acc.GetY();
	}

	void AddAccelerometerDistance(){
		AddXAccelerometerDistance();
		AddZAccelerometerDistance();
		AddYAccelerometerDistance();
	}

	double GetAccelerometerVelX(){
		return velX;
	}

	double GetAccelerometerVelZ(){
		return velZ;
	}

	double GetAccelerometerVelY(){
		return velY;
	}

	double GetAccelerometerPosX(){
		return posX;
	}

	double GetAccelerometerPosZ(){
			return posZ;
	}

	double GetAccelerometerPosY(){
		return posY;
	}


	/* Resets all encoder values */
	void Reset() {
		l_motor.Reset();
		r_motor.Reset();
	}

	/* Purpose: Shimmy to the left once
	 *
	 * Method:  Move back and to the left to a total of .5 meters (about 1.5 feet),
	 * 			then turn to the angle before movement and go straight forward
	 */
	void ShimmyLeft(int *back, int *turn, double* angle){
		double currentAngle = *angle;
		navx.ResetDisplacement();
		if(!(*back)){
			if(GetDistance() < 1000){
				SetLeft(-0.20);
				SetRight(-0.35);
			}else{
				*back = 1;
			}
		}
		navx.ResetDisplacement();
		if(!(*turn)){
			TurnAbsolute(currentAngle);
			*turn = 1;
		}

		if(navx.GetDisplacementTotal() < 0.6){
			DriveStraight(.4);
		}else{
			*back = 0;
			*turn = 0;
			*angle = 1771;
		}
	}

	void ShimmyRight(int *back, int *turn){
			double currentAngle = fmod(navx.GetAngle(),360.0);
			navx.ResetDisplacement();
			if(!(*back)){
				if(GetDistance() < 1000){
					SetLeft(-0.35);
					SetRight(-0.20);
				}else{
					*back = 1;
				}
			}
			navx.ResetDisplacement();
			if(!(*turn)){
				TurnAbsolute(currentAngle);
				*turn = 1;
			}

			if(navx.GetDisplacementTotal() < 0.6){
				DriveStraight(.4);
			}else{
				*back = 0;
				*turn = 0;
			}
		}

	/* Takes an angle value from 0 to 360 and a direction to turn and turns
	 * that direction until the angles match up
	 *
	 * Uses absolute angles
	 */
	void TurnAngle(double angle, int dir){
		/*int pass = 0;
		double prevAngle = -2;
		while(fabs(fmod(navx.GetAngle(),360.0-angle)) > 7 && pass < 10){ // While there is a greater than 7 degree threshold to the desired angle
			SetLeft(dir);
			SetRight(-dir);
			if(fabs(GetAbsoluteAngle() - prevAngle) < 7) // Checks if little to no change is occuring in angle
				pass++;							 		// to stop possible infinite loop
			prevAngle = fmod(navx.GetAngle(),360.0);
		}
		StopMotors();*/


	}

	/* Turns to an absolute angle and invokes TurnAngle() with direction being the shortest
	 * direction to get to the desired angle
	 * angle takes a value from 0 to 360
	 */
	void TurnAbsolute(double angle){
		double a = fmod(angle,360.0);			// Compensate for angles greater than 360 or negative angles
		double navang = GetAbsoluteAngle();
		TurnAngle(a , (angle-navang)/fabs(angle-navang));	// Turn angle is a, direction is equal to the sign of the difference between the desired angle and current angle
	}

	/* Takes a value from -180 to 180
	 * Invokes TurnAngle() with angle being calculated from the absolute angle and direction
	 * being the sign of the angle change
	 */
	void TurnRelative(double angle){
		double a = fmod((GetAbsoluteAngle() + angle),360.0); // turnAngle = the current angle, plus the change, modulo 360 to be 0-360
		TurnAngle(a, angle/fabs(angle));
	}

	float GetDisplacementX(){
		return navx.GetDisplacementX();
	}

	float GetDisplacementZ(){
		return navx.GetDisplacementZ();
	}

	float GetDisplacementTotal(){
		return navx.GetDisplacementTotal();
	}

	double GetAbsoluteAngle(){
		return fmod(navx.GetAngle(), 360.0);
	}

	double GetAngle(){
		return navx.GetAngle();
	}

	/* Stops all motor movement */
	void StopMotors() {
		l_motor.Set(0);
		r_motor.Set(0);
	}

	/* Sets all the motors at once */
	void SetAll(double speed) {
		l_motor.Set(speed);
		r_motor.Set(-speed);
	}

	void SetLeft(double speed){
		l_motor.Set(speed);
	}

	void SetRight(double speed){
		r_motor.Set(-speed);
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
	bool GetLeftButton(int butt)
	{
		return l_stick.GetRawButton(butt);
	}

	/* Gets the button 'butt' of the right joystick */
	bool GetRightButton(int butt)
	{
		return r_stick.GetRawButton(butt);
	}

	void ZeroNavX(){
		navx.ZeroYaw();
		ResetDisplacement();
	}

	void ResetDisplacement(){
		navx.ResetDisplacement();
	}

	double GetYaw(){
		return navx.GetYaw();
	}



	/* Purpose: Turns the robot to face a target
	 *
	 * Method : Checks if the PixyCam sees a target,
	 * 			feeds the offset of the target into the motors,
	 * 			and stops if the offset is within (-0.05, 0.05) */
	int TrackHook()
	{
		if ((int)gear_cam.GetOffset() != -2)
		{
			if((gear_cam.GetOffset() > 0.05 && gear_cam.GetOffset() < -0.05)){
				StopMotors();
				return -1; /*  -1 signifies offset is small but hook is found*/
			}

			l_motor.Set(gear_cam.GetOffset());
			r_motor.Set(-gear_cam.GetOffset());
			return 1; /*	1 signifies found object and turning towards it*/
		}
		else
		{
			StopMotors();
			return -2; /*	-2 signifies object was not found*/
		}
	}

	int TrackBoiler(){
		l_motor.Set(GetTurretXOffset());
		r_motor.Set(-GetTurretYOffset());
		/*
		 * Add Code for shoot power/y axis aiming here
		 *
		 */
		return 1;

	}

	/* Finds an average distance between the motors in case of discrepancies */
	double GetDistance()
	{
		return l_motor.GetDistance() - r_motor.GetDistance() / 2;
	}

	/* Returns the distance that the left motor has traveled */
	double GetLeftDistance()
	{
		return l_motor.GetDistance();
	}

	/* Returns the distance that the right motor has traveled */
	double GetRightDistance()
	{
		return -r_motor.GetDistance();
	}

	/* Returns the average speed of the motors */
	double GetSpeed()
	{
		return fabs((l_motor.GetSpeed())-r_motor.GetSpeed()) / 2;
	}

	void SetDist()
	{
		setDist = GetDistance();
	}


	/* Purpose: Drives 'dist' distance (either meters or encoder distance) at 'speed' speed
	 * 			while compensating for drift by factoring in rotation
	 *
	 * Method: Finds Yaw and adds it divided by 180 (the max it can be) multiplied by
	 * 			the gain value, to (hopefully) allow it to drive straight
	 */
	void DriveStraight(double speed){
		if(speed > 1) speed = 1.0;
		if(speed < -1) speed = -1.0;

		double change = navx.GetYaw()/180.0*ANGLE_GAIN;

		SetLeft(speed+change);		// CHANGE ADDITION/SUBTRACTION IF NAVX GOES NEGATIVE WHEN TURNING RIGHT
		SetRight(speed-change);
	}

	/* Purpose: Drives in 'dir' direction at 'speed' speed for 'distance' distance
	 *
	 * Method : Finds the sign of the direction,
	 * 			gets the average distance in the direction of 'sign'
	 * 			checks if the current position is less than the target distance
	 * 			checks for discrepancies within 0.1 in left and right encoder distances, indicating the robot is turning
	 * 			sets the motors to turn the opposite way
	 * 			if no discrepancies between right and left, set both motors to the same speed */
	void DriveXDistance( int speed) {
		//int sign = (r_motor.GetDistance()+l_motor.GetDistance())/fabs(r_motor.GetDistance()+l_motor.GetDistance());
		//double pos = /*sign * */ GetDistance() - setDist;
		/*if((reverse = pos < distance))
		{*/
				if(fabs(r_motor.GetDistance()) > fabs(l_motor.GetDistance()) + 0.1)
				{
						r_motor.Set(-speed * .75);
						l_motor.Set(speed);
						SmartDashboard::PutString("DB/String 9", "Drive If 1 True");
				}
				else if(fabs(r_motor.GetDistance()) + 0.1 < fabs(l_motor.GetDistance()))
				{
						r_motor.Set(-speed);
						l_motor.Set(speed * .75);
						SmartDashboard::PutString("DB/String 9", "Drive If 2 True");
				}
				else
				{
						r_motor.Set(-speed);
						l_motor.Set(speed);
						SmartDashboard::PutString("DB/String 9", "DIF: " + std::to_string(speed));
						//SmartDashboard::PutString("DB/String 10", "DIF Real: " + std::to_string());
				}
		/*}
		else // (reversedone <= 0)
		{
				StopMotors();

		}
		else if(!reverse)
		{
			SetAll(-0.7);
			reversedone--;
		}*/
	}
};

#endif /* SRC_ROBOBASE_H_ */
