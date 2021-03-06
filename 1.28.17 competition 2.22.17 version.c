#pragma config(Sensor, dgtl1,  encLiftRight,      sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  encLiftLeft,       sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  encClawRight,   sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  encClawLeft,    sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  encDriveRight,   sensorQuadEncoder)
#pragma config(Sensor, dgtl11, encDriveLeft,    sensorQuadEncoder)
#pragma config(Motor,  port1,           clawLeft,      tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           lift3,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           lift2,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           drive3,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           drive2,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           drive1,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           drive4,        tmotorVex393_MC29, openLoop, reversed)
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

#define getQuadVal() ( ( abs(SensorValue[quadRight]) + abs(SensorValue[quadLeft]) ) / 2 )

const float INTEGRAL_ACTIVE_ZONE = 50.0;
const int PIDTHRESHOLD = 30;

bool thresholdOn = true;
bool lock = false;
bool pincerMan = false;
bool pressedLastCycle = false;

/*int getQuadVal()
{
	int quadRightVal = abs(SensorValue[quadRight]);
	int quadLeftVal = abs(SensorValue[quadLeft]);
	return (quadRightVal + quadLeftVal)/2;
}*/

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

float pid(float kp, float ki, float kd, float &error, float &errorTotal, float &prevError, float sensVal)
{
	float target = SensorValue(encClawLeft);
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

task pincerPID()
{
	float kp = 9;
	float ki = 0.01;
	float kd = 0.5;
	int sensVal = 0;
	float error = 0;
	float errorTotal = 0;
	float prevError = 0;
	while(true) {
		sensVal = -SensorValue(encClawRight);
		motor[clawRight] = (pid(kp, ki, kd, error, errorTotal, prevError, sensVal));
		wait1Msec(10);
	}
}

void pincers()
{
	if (vexRT[Btn6U] == 1 || vexRT[Btn6D] == 1)
	{
		motor[clawLeft] = -127;
		lock = true;
	}
	else if (vexRT[Btn5U] == 1 || vexRT[Btn5D] == 1)
	{
		motor[clawLeft] = 127; // open claw
		lock = false;
	}
	else if (lock)
	{
		motor[clawLeft] = -30;
	}
	else
	{
		motor[clawLeft] = 0;
		thresholdOn = true;
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

void pincerManual()
{
	if (vexRT[Btn6U] == 1)
		motor[clawRight] = 127;
	else if (vexRT[Btn6D] == 1)
		motor[clawRight] = -127;
	else
		motor[clawRight] = 0;

	if (vexRT[Btn5U] == 1)
		motor[clawLeft] = 127;
	else if (vexRT[Btn5D] == 1)
		motor[clawLeft] = -127;
	else
		motor[clawLeft] = 0;
}

void pincerReset()
{
	stopTask(pincerPID);
	while(vexRT[Btn7L] == 1 && vexRT[Btn8R] == 1)
	{
		motor[clawLeft] = 70;
		motor[clawRight] = 70;
	}
		SensorValue[encClawRight] = 0;
		SensorValue[encClawLeft] = 0;
		startTask(pincerPID);
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
	//pre_auton();
	//startTask(pincerPID);
	//if(SensorValue[skills] == 1)
	//	runAutonomousSequenceSkills();
	//else
	//	if(SensorValue[autonomousSide] == 0)
	//		runAutonomousSequenceRight();
	//	else
	//		runAutonomousSequenceLeft();
}

//task usercontrol()
//{
//	pre_auton();
//	startTask(pincerPID);

//	startTask(autonomous);

//}

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
	int t1 = 0, r1 = 0, motorThreshold = 15;
	startTask(pincerPID);

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

		if (vexRT[Btn7U] == 1 && vexRT[Btn8U] == 1 && !pressedLastCycle)
		{
			pressedLastCycle = true;
		}
		if((vexRT[Btn7U] == 0 || vexRT[Btn8U] == 0) && pressedLastCycle && !pincerMan)
		{
			pressedLastCycle = false;
			pincerMan = true;
		}
		if((vexRT[Btn7U] == 0 || vexRT[Btn8U] == 0) && pincerMan && pressedLastCycle)
		{
			pressedLastCycle = false;
			pincerMan = false;
		}

		if(motor[clawLeft] == 0){
			thresholdOn = true;
		}
		else{
			thresholdOn = false;
		}

		if (pincerMan)
		{
			stopTask(pincerPID);
			pincerManual();
		}
		else{
			startTask(pincerPID);
			pincers();
		}

		if (vexRT[Btn7L] == 1 && vexRT[Btn8R] == 1)
		{
			pincerReset();
		}

		drive(r1,t1);
	}
}
