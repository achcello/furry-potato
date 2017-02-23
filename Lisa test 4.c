#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)
#pragma config(UART_Usage, UART2, uartVEXLCD, baudRate19200, IOPins, None, None)
//Variable that will carry between modes
int Program;

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          Pre-Autonomous Functions
//
// You may want to perform some actions before the competition starts. Do them in the
// following function.
//
/////////////////////////////////////////////////////////////////////////////////////////
void pre_auton()
{
	string mainBattery, backupBattery;

	//encClear();
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	//bStopTasksBetweenModes = true;
	//Leave this value alone
	int lcdScreenMin = 1;
	//This keeps track of which program you want to run
	int lcdScreen = 1;
	//Change this value to be the maximum number of programs you want on the robot
	int lcdScreenMax = 5;
	//Turns on the Backlight
	bLCDBacklight = true;
	//Copied from someone's sample code because the documentation for RobotC won't tell me anything useful
	//These should logically work, but I'm not 100% sure
	const short leftButton = 1;
	const short centerButton = 2;
	const short rightButton = 4;
	while (bIfiRobotDisabled == 1) { //Ensures this code will run ONLY when the robot is disabled
		if (nLCDButtons == leftButton) { //Scrolls to the left
			if (lcdScreenMin == lcdScreen) {
				lcdScreen = lcdScreenMax;
				wait1Msec(250);
			}
			else {
				lcdScreen --;
				wait1Msec(250);
			}
		}

		if (nLCDButtons == rightButton) { //Scrolls to the right
			if (lcdScreenMax == lcdScreen) {
				lcdScreen = lcdScreenMin;
				wait1Msec(250);
				} else {
				lcdScreen++;
				wait1Msec(250);
			}
		}

		if (lcdScreen == 1 && Program != 1) {
			displayLCDCenteredString (0, "Autonomous Right"); //Name the first program here
			displayLCDCenteredString (1, "<    Select    >"); //Name the first program here
			if (nLCDButtons == centerButton) {
				Program = lcdScreen; //Sets the Program to the one on-screen
				displayLCDCenteredString (0, "Autonomous Right");
				displayLCDCenteredString (1, "SELECTED");
				wait1Msec(1500);
			}
		}

		else if (lcdScreen == 1 && Program == 1) {
			displayLCDCenteredString (0, "Autonomous Right"); //We use brackets to mark which program we have chosen
			displayLCDCenteredString (1, "<  [SELECTED]  >"); //So that while we're scrolling, we can have one marked
		}

		else if (lcdScreen == 2 && Program != 2) {
			displayLCDCenteredString (0, " Autonomous Left"); //Name the second program here
			displayLCDCenteredString (1, "<    Select    >"); //Name the second program here
			if (nLCDButtons == centerButton) {
				Program = lcdScreen; //Sets the Program to the one on-screen
				displayLCDCenteredString (0, " Autonomous Left");
				displayLCDCenteredString (1, "SELECTED");
				wait1Msec(1500);
			}
		}

		else if (lcdScreen == 2 && Program == 2) {
			displayLCDCenteredString (0, " Autonomous Left"); //We use brackets to mark which program we have chosen
			displayLCDCenteredString (1, "<  SELECETED!  >"); //So that while we're scrolling, we can have one marked
		}

		else if (lcdScreen == 3 && Program != 3) {
			displayLCDCenteredString (0, " Autonomous Mid"); //Name the third program here
			displayLCDCenteredString (1, "<    Select    >"); //Name the third program here
			if (nLCDButtons == centerButton) {
				Program = lcdScreen;                       //Sets the Program to the one on-screen
				displayLCDCenteredString (0, " Autonomous Mid");
				displayLCDCenteredString (1, "SELECTED");
				wait1Msec(1500);
			}
		}

		else if (lcdScreen == 3 && Program == 3) {
			displayLCDCenteredString (0, " Autonomous Mid"); //We use brackets to mark which program we have chosen
			displayLCDCenteredString (1, "<  SELECTED!  >"); //So that while we're scrolling, we can have one marked
		}

		else if (lcdScreen == 4 && Program != 4) {
			displayLCDCenteredString (0, "Battery Voltages");
			displayLCDCenteredString (1, "<    Select    >");

			if (nLCDButtons == centerButton) {
				displayLCDString(0, 0, "Primary: ");
				sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0,'V'); //Build the value to be displayed
				displayNextLCDString(mainBattery);                                  //Display the Backup battery voltage
				displayLCDString(1, 0, "Backup: ");
				sprintf(backupBattery, "%1.2f%c", BackupBatteryLevel/1000.0, 'V');    //Build the value to be displayed
				displayNextLCDString(backupBattery);
				wait1Msec(10000);
			}
		}

		else if (lcdScreen == 4 && Program == 4) {
			displayLCDCenteredString (0, "Battery Voltages"); //We use brackets to mark which program we have chosen
			displayLCDCenteredString (1, "<    Select    >"); //So that while we're scrolling, we can have one marked
		}
	}
}

task autonomous(){
}
task usercontrol(){}
