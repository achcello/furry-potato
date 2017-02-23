#pragma config(UART_Usage, UART2, uartVEXLCD, baudRate19200, IOPins, None, None)
//#pragma config(Sensor, in1,    ,               sensorAnalog) // need to google how exactly I need to set up the power expander



const short leftButton = 1;
const short centerButton = 2;
const short rightButton = 4;

//Wait for Press--------------------------------------------------
void waitForPress()
{
	while(nLCDButtons == 0){}
	wait1Msec(5);
}
//----------------------------------------------------------------

//Wait for Release------------------------------------------------
void waitForRelease()
{
	while(nLCDButtons != 0){}
	wait1Msec(5);
}
//----------------------------------------------------------------

task main()
{

	bLCDBacklight = true;                                    // Turn on LCD Backlight
	string mainBattery, backupBattery;

	int count = 0;                                           //Declare count variable to keep track of our choice

	//------------- Beginning of User Interface Code ---------------
	//Clear LCD
	clearLCDLine(0);
	clearLCDLine(1);



		//Loop while center button is not pressed
		while(nLCDButtons != centerButton)
		{
			//Switch case that allows the user to choose from 5 different options
			switch(count){

			case 0:
				//Display first choice
				displayLCDCenteredString(0, "Autonomous 1");
				displayLCDCenteredString(1, "<    Select    >");
				waitForPress();
				//Increment or decrement "count" based on button press
				if(nLCDButtons == leftButton)
				{
					waitForRelease();
					count = 4;
				}

				if(nLCDButtons == rightButton)
				{
					waitForRelease();
					count++;
	}
				break;


			case 1:
				//Display second choice
				displayLCDCenteredString(0, "Autonomous 2");
				displayLCDCenteredString(1, "<    Select    >");
				waitForPress();
				//Increment or decrement "count" based on button press
				if(nLCDButtons == leftButton)
				{
					waitForRelease();
					count--;
				}
				else if(nLCDButtons == rightButton)
				{
					waitForRelease();
					count++;
				}
				break;


			case 2:
				//Display third choice
				displayLCDCenteredString(0, "Autonomous 3");
				displayLCDCenteredString(1, "<    Select    >");
				waitForPress();
				//Increment or decrement "count" based on button press
				if(nLCDButtons == leftButton)
				{
					waitForRelease();
					count--;
				}
				else if(nLCDButtons == rightButton)
				{
					waitForRelease();
					count++;
				}
				break;


			case 3:
				//Display fourth choice
				displayLCDCenteredString(0, "Autonomous 4");
				displayLCDCenteredString(1, "<    Select    >");
				waitForPress();
				//Increment or decrement "count" based on button press
				if(nLCDButtons == leftButton)
				{
					waitForRelease();
					count--;
				}
				else if(nLCDButtons == rightButton)
				{
					waitForRelease();
					count++;
				}
				break;


			case 4:
				//Display fifth choice
				displayLCDCenteredString(0, "Battery Voltages");
				displayLCDCenteredString(1, "<    Select    >");
				waitForPress();
				//Increment or decrement "count" based on button press
				if(nLCDButtons == leftButton)
				{
					waitForRelease();
					count--;
				}
				else if(nLCDButtons == rightButton)
				{
					waitForRelease();
					count = 0;
				}
				break;


				//------------- End of User Interface Code ---------------------
				//------------- Beginning of Robot Movement Code ---------------
				//Clear LCD
				clearLCDLine(0);
				clearLCDLine(1);
				//Switch Case that actually runs the user choice
				switch(count){
				case 0:
					//If count = 0, run the code correspoinding with choice 1
					displayLCDCenteredString(0, "Autonomous 1");
					displayLCDCenteredString(1, "Selected");
					wait1Msec(3000);                            // Robot runs previous code for 3000 milliseconds before moving on
					break;

				case 1:
					//If count = 1, run the code correspoinding with choice 2
					displayLCDCenteredString(0, "Autonomous 2");
					displayLCDCenteredString(1, "Sellected");
					wait1Msec(1000);                            // Robot runs previous code for 3000 milliseconds before moving on
					break;

				case 2:
					//If count = 2, run the code correspoinding with choice 3
					displayLCDCenteredString(0, "Autonomous 3");
					displayLCDCenteredString(1, "Selected");
					wait1Msec(1000);                            // Robot runs previous code for 3000 milliseconds before moving on
					break;

				case 3:
					//If count = 3, run the code correspoinding with choice 4
					displayLCDCenteredString(0, "Autonomous 4");
					displayLCDCenteredString(1, "Selected");
					wait1Msec(1000);                            // Robot runs previous code for 3000 milliseconds before moving on
					break;

				case 4:
					//If count = 4, run the code correspoinding with choice 5
					displayLCDString(0, 0, "Primary: ");
					sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0,'V'); //Build the value to be displayed
					displayNextLCDString(mainBattery);

					//Display the Backup battery voltage
					displayLCDString(1, 0, "Backup: ");
					sprintf(backupBattery, "%1.2f%c", BackupBatteryLevel/1000.0, 'V');    //Build the value to be displayed
					displayNextLCDString(backupBattery);
					wait1Msec(10000);                            // Robot runs previous code for 3000 milliseconds before moving on
					break;

				default:
					displayLCDCenteredString(0, "No valid choice");
					displayLCDCenteredString(1, "was made!");
					break;
				}
				//------------- End of Robot Movement Code -----------------------
			}
		}
	}
