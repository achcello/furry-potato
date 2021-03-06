#pragma config(Sensor, dgtl1,  encLiftRight,   sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  encLiftLeft,    sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  encClawRight,   sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  encClawLeft,    sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  encDriveRight,  sensorQuadEncoder)
#pragma config(Sensor, dgtl11, encDriveLeft,   sensorQuadEncoder)
#pragma config(Motor,  port1,           clawLeft,      tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           lift3,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           lift2,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           leftWheel,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           rightWheel,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           lift4,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           lift1,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          clawRight,     tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//for Margaret Thatcher, the robot

#pragma platform(VEX)

#pragma competitionControl(Competition)
#pragma autonomousDuration(15)
#pragma userControlDuration(105)

#include "Vex_Competition_Includes.c"

#include "bananananananana.c"

const float INTEGRAL_ACTIVE_ZONE = 50.0;
const int PIDTHRESHOLD = 30;

bool thresholdOn = true;

bool pincerMan = false;
bool pressedLastCycle = false;

//drive exactly constants
int driveThreshold = 15;
bool driveThresholdOn = true;
int distance = 0;
int angle = 0;

//lift constants
int liftHeight = 0;

//claw constants
bool clawPIDRunning = true;
float clawSetPoint = 0;
bool lock = false;

//autonomous constants
int autonomousSequence = 1;
int autonomousCheckPoint = 3;
bool autonomousRunning = false;

void drive(int rot1, int trans1)
{
	motor[rightWheel] = 0.75*(trans1 - rot1);
	motor[leftWheel] = 0.75*(trans1 + rot1);
}

void lift(int liftPower)
{
	motor[lift2] = liftPower;
	motor[lift1] = liftPower;
	motor[lift3] = liftPower;
	motor[lift4] = liftPower;
}

task driveExactly()
{

	//Clear sensors
	SensorValue[encDriveLeft] = 0;
	SensorValue[encDriveRight] = 0;

	//DISTANCE ONLY
	//drives the distance in inches
	//forwards is positive
	//27.7 ticks per inch
	if(angle == 0 && distance != 0){
		float kp = 0.40;
		driveThresholdOn = true;

		int direction = 1;
		if(distance < 0)
			direction = -1;

		int target = distance * 27.7;

		while(abs(target - SensorValue[encDriveLeft]) > 15){
			int driveMotorSpeed = abs(target - SensorValue[encDriveLeft]) * kp;
			if(driveMotorSpeed > 127)//delete this if we remove 0.75 from drive function
				driveMotorSpeed = 127;
			if(driveMotorSpeed < driveThreshold && driveThresholdOn){
				driveMotorSpeed = driveThreshold;
			}
			drive(0,direction * driveMotorSpeed);
		}

		direction = direction * -1;

		for(int x = 0; x <= 80; x++){
			drive(0,x * direction);
			wait1Msec(2);
		}

		drive(0,0);
	}

	//ANGLE ONLY
	//turns the robot based on angle
	//clockwise is positive
	//3.66 ticks per angle
	if(angle != 0 && distance == 0){
		float kp = 2.0;
		driveThresholdOn = true;

		int direction = 1;
		if(angle < 0)
			direction = -1;

		int target = angle * 3.66;

		while(abs(target - SensorValue[encDriveLeft]) > 10){
			int driveMotorSpeed = abs(target - SensorValue[encDriveLeft]) * kp;
			if(driveMotorSpeed > 127)//delete this if we remove the 0.75 from drive function
				driveMotorSpeed = 127;
			if(driveMotorSpeed < driveThreshold && driveThresholdOn){
				driveMotorSpeed = driveThreshold;
			}
			drive(direction * driveMotorSpeed, 0);
		}

		direction = direction * -1;

		drive(127 * direction, 0);
		wait1Msec(100);

		drive(0,0);
	}
	autonomousCheckPoint++;
}

int getLiftHeight()
{
	int quadRightVal = abs(SensorValue[encLiftRight]);
	//int quadLeftVal = abs(SensorValue[encLiftLeft]);
	return (quadRightVal); //+ quadLeftVal)/2;
}

task liftExactly(){
	int direction;
	if(liftHeight > getLiftHeight())
		direction = 1;
	else
		direction = -1;
	while(abs(liftHeight - getLiftHeight()) > 10)
		lift(127 * direction);
	lift(0);
	autonomousCheckPoint++;
}

float pid(float kp, float ki, float kd, float target, float &error, float &errorTotal, float &prevError, float sensVal)
{
	error = target - sensVal;
	errorTotal += error;
	float deltaError = error - prevError;

	//set components of pid
	float proportional = kp * error;
	float integral = ki * errorTotal;
	float derivative = kd * deltaError;

	// Prevent Integral from Getting Too High or Too Low
	if (integral > INTEGRAL_ACTIVE_ZONE) {
		integral = INTEGRAL_ACTIVE_ZONE;
	}
	if (integral < -INTEGRAL_ACTIVE_ZONE) {
		integral = -INTEGRAL_ACTIVE_ZONE;
	}

	prevError = error;

	return (proportional + integral + derivative);
}

task pincerPID()
{
	const float L_claw_kp = 6.75;
	const float L_claw_ki = 0.0075;
	const float L_claw_kd = 0.375;

	const float R_claw_kp = 6.75;
	const float R_claw_ki = 0.0075;
	const float R_claw_kd = 0.375;

	float L_claw_error = 0;
	float L_claw_prevError = 0;
	float L_claw_errorTotal = 0;

	float R_claw_error = 0;
	float R_claw_prevError = 0;
	float R_claw_errorTotal = 0;

	float L_claw_sensVal, R_claw_sensVal;

	while (true)
	{
		if(clawPIDRunning)
		{
			L_claw_sensVal = SensorValue(encClawLeft);
			R_claw_sensVal = SensorValue(encClawRight);

			motor[clawLeft] = pid(L_claw_kp, L_claw_ki, L_claw_kd, clawSetPoint, L_claw_error, L_claw_errorTotal, L_claw_prevError, L_claw_sensVal);
			motor[clawRight] = pid(R_claw_kp, R_claw_ki, R_claw_kd, clawSetPoint, R_claw_error, R_claw_errorTotal, R_claw_prevError, R_claw_sensVal);
		}
		if(abs(L_claw_sensVal - clawSetPoint) < 10 && abs(R_claw_sensVal - clawSetPoint) < 10 && autonomousRunning){
			autonomousCheckPoint++;
			break;
		}
		sleep(20);
	}
}

task updatePincerUserControl()
{
	int CLsens, CRsens;
	int lastTime = 0;
	int CLAW_MIN_ROTATION = 100000000;
	int CLAW_MAX_ROTATION = -100000000;
	while(true)
	{
		CLsens = SensorValue(encClawLeft);
		CRsens = SensorValue(encClawRight);

		if (vexRT[Btn8R] == 1 || vexRT[Btn8DXmtr2] == 1) {
			clawPIDRunning = false;
			// to give other thread time,  stop pid
			sleep(30);
			motor[clawLeft] = 0;
			motor[clawRight] = 0;
			clawSetPoint = (CLsens + CRsens) / 2.0;
			lastTime = time1[T1];
		}
		else {
			if (vexRT[Btn5U] == 1 || vexRT[Btn6UXmtr2] == 1)
			{
				clawPIDRunning = false;
				// open claw fast
				if(CLsens < CLAW_MIN_ROTATION) motor[clawLeft] = 127;
				if(CRsens < CLAW_MIN_ROTATION) motor[clawRight] = 127;
				clawSetPoint = (CLsens + CRsens) / 2.0;
				lastTime = time1[T1];
				lock = false;
			}
			else if (vexRT[Btn6U] == 1 || vexRT[Btn6DXmtr2] == 1)
			{
				clawPIDRunning = false;
				// close claw fast
				if(CLsens > CLAW_MAX_ROTATION) motor[clawLeft] = -127;
				if(CRsens > CLAW_MAX_ROTATION) motor[clawRight] = -127;
				clawSetPoint = (CLsens + CRsens) / 2.0;
				lastTime = time1[T1];
				lock = true;
			}
			/*else if (vexRT[Btn6D] == 1 || vexRT[Btn5DXmtr2] == 1)
			{
			clawPIDRunning = false;
			// close claw slow
			if(CLsens > CLAW_MAX_ROTATION) motor[CL] = 30;
			if(CRsens > CLAW_MAX_ROTATION) motor[CR] = 30;
			clawSetPoint = (CLsens + CRsens) / 2.0;
			lastTime = time1[T1];
			}
			else if (vexRT[Btn5D] == 1 || vexRT[Btn5UXmtr2] == 1)
			{
			clawPIDRunning = false;
			// open claw slow
			if(CLsens < CLAW_MIN_ROTATION) motor[CL] = -30;
			if(CRsens < CLAW_MIN_ROTATION) motor[CR] = -30;
			clawSetPoint = (CLsens + CRsens) / 2.0;
			lastTime = time1[T1];
			}*/
			else
			{
				if(time1[T1] - lastTime < 300) {
					clawSetPoint = (CLsens + CRsens) / 2.0;
				}
				if(!lock){
					clawPIDRunning = true;
				}
				else
				{
					motor[clawLeft] = -30;
					motor[clawRight] = -30;
				}
			}
		}

		sleep(20);
	}
}

task lockPincers()
{
	while(lock){
		motor[clawLeft] = -30;
		motor[clawRight] = -30;
	}
}

void runSequenceStep(int driveTrans, int driveRot, int clawValue, int liftValue, int lockValue)
{
	distance = driveTrans;
	angle = driveRot;
	liftHeight = liftValue;
	clawSetPoint = clawValue;
	startTask(driveExactly);
	startTask(liftExactly);
	startTask(pincerPID);
	if(lockValue == 1){
		lock = true;
		startTask(lockPincers);
	}
}

void runAutonomousRight()
{
	//make this consice so that we only use one function for all possible autnomous
	int numRows = sizeof(autonomousRight) / sizeof(autonomousRight[0]);
	for(int x = 0; x < numRows; x++){
		autonomousCheckPoint = 0;
		runSequenceStep(autonomousRight[x][0],autonomousRight[x][1],autonomousRight[x][2],autonomousRight[x][3], autonomousRight[x][4]);
		while(autonomousCheckPoint != 3){
			//waits for all task to finish

		}
		wait1Msec(autonomousRight[x][5]);
		lock = false;
	}
}

void pre_auton()
{
	SensorValue[encLiftLeft] = 0;
	SensorValue[encLiftRight] = 0;
	SensorValue[encClawRight] = 0;
	SensorValue[encClawLeft] = 0;
	SensorValue[encDriveLeft] = 0;
	SensorValue[encDriveRight] = 0;
}

task autonomous()
{
	pre_auton();
	autonomousRunning =true;
	if(autonomousSequence == 1)
		runAutonomousRight();
}

task usercontrol()
{
	pre_auton();
	autonomousRunning = true;
	clawSetPoint = -80;
	startTask(pincerPID);

	//pre_auton();
	//autonomousRunning =false;
	//int t1 = 0, r1 = 0, motorThreshold = 15;
	//startTask(pincerPID);
	//startTask(updatePincerUserControl);

	//while(true){
	//	if(vexRT[Btn7R] == 1){
	//		//startTask(autonomous);
	//	}

	//	if(abs(vexRT[Ch1]) > motorThreshold)
	//		r1 = vexRT[Ch1];
	//	else
	//		r1 = 0;

	//	if(abs(vexRT[Ch2]) > motorThreshold)
	//		t1 = vexRT[Ch2];
	//	else
	//		t1 = 0;

	//	if (abs(vexRT[Ch3]) > motorThreshold)
	//	{
	//		lift(vexRT[Ch3]);
	//	}
	//	else
	//	{
	//		lift(0);
	//	}

	//	drive(r1,t1);
	//}
}
