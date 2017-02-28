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

int leftAdjustment = 0;
int rightAdjustment = 0;

int r1 = 0;
int t1 = 0;


bool runPID = false;

void drive(int rot1, int trans1)
{
	motor[rightWheel] = 0.75*(trans1 - rot1) + rightAdjustment;
	motor[leftWheel] = 0.75*(trans1 + rot1) + leftAdjustment;
}

void lift(int liftPower)
{
	motor[lift2] = liftPower;
	motor[lift1] = liftPower;
	motor[lift3] = liftPower;
	motor[lift4] = liftPower;
}

void clearEncoders()
{
	SensorValue[encLiftLeft] = 0;
	SensorValue[encLiftRight] = 0;
	SensorValue[encClawRight] = 0;
	SensorValue[encClawLeft] = 0;
	SensorValue[encDriveLeft] = 0;
	SensorValue[encDriveRight] = 0;
}

task drivePID(){
	float error = 0;
	float prevError = 0;

	float kp = 0.25;
	float kd = 2.0;

	float adjustment = 0;

	float target;

	int left;
	int right;

	while(true){
		if(runPID){
			right = -1 * SensorValue[encDriveRight];
			left = SensorValue[encDriveLeft];

			wait1Msec(20);

			right = -1 * SensorValue[encDriveRight] - right;
			left = SensorValue[encDriveLeft] - left;
			error = abs(right) - abs(left);

			adjustment = abs(error) * kp + (error - prevError) * kd;

			leftAdjustment = 0;
			rightAdjustment = 0;

			if(error > 0){
				if(motor[rightWheel] > 0)
					rightAdjustment = -1 * adjustment;
				else
					rightAdjustment = adjustment;
			}
			else{
				if(motor[leftWheel] > 0)
					leftAdjustment = -1 * adjustment;
				else
					leftAdjustment = adjustment;
			}

			prevError = error;
		}
	}
}

task main()
{

	clearEncoders();
	startTask(drivePID);
	int motorThreshold = 15;

	while(true){
		if(vexRT[Btn7R] == 1 && runPID){
			runPID = false;
		}
		else
			if(vexRT[Btn7R] == 1 && !runPID){
			runPID = true;
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
