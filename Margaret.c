#pragma config(Sensor, dgtl1,  quadRight,      sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  quadLeft,       sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  encClawRight,   sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  encClawLeft,    sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  encRightSide,   sensorQuadEncoder)
#pragma config(Sensor, dgtl11, encLeftSide,    sensorQuadEncoder)
#pragma config(Motor,  port1,           clawLeft,      tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           lift2,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           lift1,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           drive3,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           drive4,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           drive1,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           drive2,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           lift3,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           lift4,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          clawRight,     tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//for Margaret Thatcher, the robot

#pragma platform(VEX)

#pragma competitionControl(Competition)
#pragma autonomousDuration(15)
#pragma userControlDuration(105)

#include "Vex_Competition_Includes.c"

#define getQuadVal() ( ( abs(SensorValue[quadRight]) + abs(SensorValue[quadLeft]) ) / 2 )

#define CR clawRight //reconcile differences in the port in #pragma with #define
#define CL clawLeft
#define CLenc encClawLeft
#define CRenc encClawRight

const float INTEGRAL_ACTIVE_ZONE = 50.0;
const int PIDTHRESHOLD = 30;

bool thresholdOn = true;
bool clawPIDRunning = true;
bool pressedLastCycle = false;
float clawTarget;

int getClawAvg()
{
	while (true) {
		float quadRightVal = abs(SensorValue[quadRight]);
		float quadLeftVal = abs(SensorValue[quadLeft]);
		clawTarget = (quadRightVal + quadLeftVal)/2;
	}

}

void drive(int rot1, int trans1)
{
	motor[drive1] = 0.75*(-1*trans1 - -1*rot1);
	motor[drive2] = 0.75*(-1*trans1 + -1*rot1);
	motor[drive3] = 0.75*(-1*trans1 + -1*rot1);
	motor[drive4] = 0.75*(-1*trans1 - -1*rot1);
}

void lift(int liftPower)
{
	motor[lift2] = liftPower;
	motor[lift1] = -liftPower;
	motor[lift3] = liftPower;
	motor[lift4] = -liftPower;
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

	if((0.75*(abs(proportional + integral + derivative)) < PIDTHRESHOLD) && thresholdOn)
	{
		return 0;
	}
	else
	{
		return -0.75*(proportional + integral + derivative);
	}

}

void pre_auton()
{
	SensorValue[quadRight] = 0;
	SensorValue[quadLeft] = 0;
	SensorValue[encClawRight] = 0;
	SensorValue[encClawLeft] = 0;
}

task pincerPID()
{
	const float L_claw_kp = 3.3;
	const float L_claw_ki = 0.0000001;
	const float L_claw_kd = 2;

	const float R_claw_kp = 3.3;
	const float R_claw_ki = 0.0000001;
	const float R_claw_kd = 2;

	float L_claw_error = 0;
	float L_claw_prevError = 0;
	float L_claw_errorTotal = 0;

	float R_claw_error = 0;
	float R_claw_prevError = 0;
	float R_claw_errorTotal = 0;

	float L_claw_sensVal, R_claw_sensVal;

	float motorLClawOutput, motorRClawOutput;

	while (true)
	{
		if(clawPIDRunning)
		{
			L_claw_sensVal = SensorValue(encClawLeft);
			R_claw_sensVal = SensorValue(encClawRight);

			motorLClawOutput = pid(L_claw_kp, L_claw_ki, L_claw_kd, clawTarget, L_claw_error, L_claw_errorTotal, L_claw_prevError, L_claw_sensVal);
			motorRClawOutput = pid(R_claw_kp, R_claw_ki, R_claw_kd, clawTarget, R_claw_error, R_claw_errorTotal, R_claw_prevError, R_claw_sensVal);

			motor[CL] = motorLClawOutput;
			motor[CR] = motorRClawOutput;
		}
		sleep(20);
	}
}


//task autonomous()
//{
//	pre_auton();
//	startTask(pincerPID);
//	startTask(clawAuwait1wait1Msec;
//	startTask(driveAuton);
//	startTask(liftAuton);
//	sleep(1500);
//}

void runSequenceStep(int driveRot, int driveTrans, int clawValue, int liftValue)
{
	drive(driveRot, driveTrans);
	motor[clawLeft] = -1*clawValue;
	lift(liftValue);
}

void runAutonomousSequenceRight()
{
	int sequence[21][5] = {
		//{rotation,translation,claw,lift,time in milliseconds}
		{0,127,0,0,150},//run to fence
		{0,127,127,127,800},//run up to fence, with claws opening and lift going up
		{0,127,0,127,950},//keep going but claws stop
		{0,-127,0,0,400},//go back
		{0,-127,0,-127,900},//go back and lower lift
		{0,0,0,-127,200},//lower lift
		{-127,0,0,0,500},//turn towards cube
		{0,127,0,0,800},//drive towards cube
		{0,0,127,0,1000},//grab cube
		{0,127,30,127,300},//drive forwards and lift up
		{127,0,30,127,600},//lift cube while turning to fence
		{0,0,30,127,600},//lift cube
		{0,127,30,127,1050},//lift cube to fence height and drive to fence
		{0,0,-127,0,800},//drop cube
		{0,-127,0,0,500},//drive backwards
		{0,-127,0,-127,600},//drive backwards and lift down
		{0,127,0,0,600},//drive fowards
		{0,127,0,127,650},//drive fowards and lift up
		{0,-127,0,0,700},//drive backwards
		{0,-127,0,-127,500},//drive backwards and lift down
		{0,0,0,0,0}//stop
	};

	for(int x = 0; x < 21; x++){
		runSequenceStep(sequence[x][0],sequence[x][1],sequence[x][2],sequence[x][3]);
		wait1Msec(sequence[x][4]);
	}
}

void runAutonomousSequenceLeft()
{
	int sequence[21][5] = {
		//{rotation,translation,claw,lift,time in milliseconds}
		{0,127,0,0,150},//run to fence
		{0,127,127,127,800},//run up to fence, with claws opening and lift going up
		{0,127,0,127,950},//keep going but claws stop
		{0,-127,0,0,400},//go back
		{0,-127,0,-127,900},//go back and lower lift
		{0,0,0,-127,200},//lower lift
		{127,0,0,0,500},//turn towards cube
		{0,127,0,0,800},//drive towards cube
		{0,0,127,0,1000},//grab cube
		{0,127,30,127,300},//drive forwards and lift up
		{-127,0,30,127,600},//lift cube while turning to fence
		{0,0,30,127,600},//lift cube
		{0,127,30,127,1050},//lift cube to fence height and drive to fence
		{0,0,-127,0,800},//drop cube
		{0,-127,0,0,500},//drive backwards
		{0,-127,0,-127,600},//drive backwards and lift down
		{0,127,0,0,700},//drive fowards
		{0,127,0,127,550},//drive fowards and lift up
		{0,-127,0,0,700},//drive backwards
		{0,-127,0,-127,500},//drive backwards and lift down
		{0,0,0,0,0}//stop
	};

	for(int x = 0; x < 21; x++){
		runSequenceStep(sequence[x][0],sequence[x][1],sequence[x][2],sequence[x][3]);
		wait1Msec(sequence[x][4]);
	}
}

void runAutonomousSequenceSkills()
{
	int sequence[34][5] = {
		//{rotation,translation,claw,lift,time in milliseconds}
		{0,127,0,0,150},//run to fence
		{0,127,127,127,800},//run up to fence, with claws opening outwards(closing inwards) and lift going up
		{0,127,0,127,1050},//keep going but claws stop
		{0,-127,0,0,400},//go back
		{0,-127,0,-127,900},//go back and lower lift
		{0,0,0,-127,200},//lower lift
		{127,0,0,0,500},//turn towards cube
		{0,127,0,0,800},//drive towards cube
		{0,0,127,0,1000},//grab cube
		{-127,0,30,127,700},//lift cube while turning to fence
		{0,0,30,127,600},//lift cube
		{0,127,30,127,1050},//lift cube to fence height and drive to fence
		{0,0,-127,0,800},//drop cube
		{0,-127,0,0,450},//drive backwards
		{0,-127,0,-127,1000},//drive backwards and lift down
		{0,127,0,127,400},//drive fowards and lift up
		{0,127,0,0,800},//drive fowards
		{0,127,0,127,300},//drive fowards and lift up
		{0,-127,0,0,400},//drive backwards
		{0,-127,0,-127,800},//drive backwards and lift down
		{0,127,0,0,1000},//drive forwards
		{0,0,127,0,1000},//close claws
		{0,-127,0,127,1000},//drive backwards and lift up
		{0,127,0,127,1000},//drive forwards and lift up
		{0,0,-127,0,800},//drop cube
		{0,-127,0,0,400},//drive backwards
		{-127,-127,0,-127,800},//drive backwards and lift down and turn towards back wall
		{0,127,0,0,70},//drive fowards
		{-127,127,0,0,700},//turn towards back wall and move fowards
		{-127,0,0,0,700},//turn towards back wall
		{0,127,0,0,650},//drive towards back wall
		{0,0,127,0,1000},//close claw on stars
		{0,-127,0,0,1500},//drive backwards
		{0,0,0,0,0}//stop
	};

	for(int x = 0; x < 34; x++){
		runSequenceStep(sequence[x][0],sequence[x][1],sequence[x][2],sequence[x][3]);
		wait1Msec(sequence[x][4]);
	}
}

task timeAuton()
{
	wait1Msec(15000);
	stopTask(autonomous);
	//startTask(usercontrol);
}

task autonomous()
{
	//startTask(timeAuton);
	pre_auton();
	startTask(pincerPID);
	if(SensorValue[skills] == 1)
		runAutonomousSequenceSkills();
	else
		if(SensorValue[autonomousSide] == 0)
			runAutonomousSequenceRight();
		else
			runAutonomousSequenceLeft();
}

//task usercontrol()
//{
//	pre_auton();
//	startTask(pincerPID);

//	startTask(autonomous);

//}

// Controls the Claw of the Robot During the User Control Portion of the Competition
task updatePincerUserControl()
{
	int CLsens, CRsens;

	int lastTime = 0;
	while(true)
	{
		CLsens = SensorValue(CLenc);
		CRsens = SensorValue(CRenc);

		if (vexRT[Btn8R] == 1 || vexRT[Btn8DXmtr2] == 1) {
			clawPIDRunning = false;
			// to give other thread time,  stop pid
			sleep(30);
			motor[CL] = 0;
			motor[CR] = 0;
			clawSetPoint = (CLsens + CRsens) / (2.0 * CLAW_GEAR_RATIO);
			lastTime = time1[T1];
		}
		else {
			if (vexRT[Btn5U] == 1 || vexRT[Btn6UXmtr2] == 1)
			{
				clawPIDRunning = false;
				// open claw fast
				if(CLsens > CLAW_MIN_ROTATION) motor[CL] = -127;
				if(CRsens > CLAW_MIN_ROTATION) motor[CR] = -127;
				clawSetPoint = (CLsens + CRsens) / (2.0 * CLAW_GEAR_RATIO);
				lastTime = time1[T1];
			}
			else if (vexRT[Btn6U] == 1 || vexRT[Btn6DXmtr2] == 1)
			{
				clawPIDRunning = false;
				// close claw fast
				if(CLsens < CLAW_MAX_ROTATION) motor[CL] = 127;
				if(CRsens < CLAW_MAX_ROTATION) motor[CR] = 127;
				clawSetPoint = (CLsens + CRsens) / (2.0 * CLAW_GEAR_RATIO);
				lastTime = time1[T1];
			}
			else if (vexRT[Btn6D] == 1 || vexRT[Btn5DXmtr2] == 1)
			{
				clawPIDRunning = false;
				// close claw slow
				if(CLsens < CLAW_MAX_ROTATION) motor[CL] = 30;
				if(CRsens < CLAW_MAX_ROTATION) motor[CR] = 30;
				clawSetPoint = (CLsens + CRsens) / (2.0 * CLAW_GEAR_RATIO);
				lastTime = time1[T1];
			}
			else if (vexRT[Btn5D] == 1 || vexRT[Btn5UXmtr2] == 1)
			{
				clawPIDRunning = false;
				// open claw slow
				if(CLsens > CLAW_MIN_ROTATION) motor[CL] = -30;
				if(CRsens > CLAW_MIN_ROTATION) motor[CR] = -30;
				clawSetPoint = (CLsens + CRsens) / (2.0 * CLAW_GEAR_RATIO);
				lastTime = time1[T1];
			}
			else
			{
				if(time1[T1] - lastTime < 300) {
					clawSetPoint = (CLsens + CRsens) / (2.0 * CLAW_GEAR_RATIO);
				}
				clawPIDRunning = true;
			}
		}

		sleep(20);
	}
}

//STUFF TO WORK ON IN CODE:
//SOMEHOW GET THIRD STAR ON FIRST FENCE
//GET SENSORS TO WORK
//GET OLD CLAW BACK??
//learn how to find length of array to put in for loop!!!!!!!!!!!
//sweep back two stars with cube
//collect encoder data with test and debug with writeDebugStreamLine
//find out whats overloading robot?? repeatedly starting tasks in ueser control?

task usercontrol()
{
	pre_auton();
	int t1 = 0, r1 = 0, motorThreshold = 25;
	startTask(getClawAvg);
	startTask(pincerPID);
	startTask(updatePincerUserControl);

	while(true){
		if(vexRT[Btn7R] == 1){
			//startTask(autonomous);
		}

		if(abs(vexRT[Ch1]) > motorThreshold)
			r1 = vexRT[Ch1];
		else
			r1 = 0;

		if(abs(vexRT[Ch2]) > motorThreshold)
			t1 = vexRT[Ch2];
		else
			t1 = 0;

		if (abs(vexRT[Ch3]) > motorThreshold)
		{
			lift(vexRT[Ch3]);
		}
		else
		{
			lift(0);
		}
		drive(r1,t1);
	}
}