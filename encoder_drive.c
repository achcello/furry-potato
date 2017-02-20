void driveExactly(int distance)
{
	//drives forward the distance in inches
	//encoder resolution = 360
	//wheel diameter = 4
	//28.6479 counts per inch of linear translation

	int multiplier = -1;
	if(distance < 0)
		multiplier = 1;

	int target = distance * 28.6479;

	while(abs(target - SensorValue[encDriveLeft] * -1) > 720){
		drive(0,multiplier * 127);
	}


	while(abs(target - SensorValue[encDriveLeft] * -1) > 360){
		drive(0,multiplier * 50);
	}

	while(abs(target - SensorValue[encDriveLeft] * -1) > 10){
		multiplier = multiplier * -1;
		drive(0,127 * multiplier);
	}
	drive(0,0);
}


void turnExactly(int angle)
{
	//clockwise is positive angle
	//3.66 encoder ticks per angle
	int target = 3.66 * angle;
	float ratio = 180.0/angle;

	int multiplier = -1;
	if(angle < 0)
		multiplier = 1;


	while(abs(target - nMotorEncoder[drive3] * -1) > 150 * ratio){
		drive(multiplier * 127,0);
	}

	while(abs(target - nMotorEncoder[drive3] * -1) > 75 / ratio){
		drive(multiplier * 63,0);
	}

	while(abs(target - nMotorEncoder[drive3] * -1) > 10){
		multiplier = -1 * multiplier;
		drive(127 * multiplier,0);
	}
	drive(0,0);
}