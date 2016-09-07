/** @file 	nav.c
	@author K. Arik & F. Sullivan
	@date 	21 April 2015
	@brief 	Support for navagation buttons with debouncing.
*/

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "drivers/rit128x96x4.h"
#include "encoder.h"
#include "stdio.h"
#include "stdlib.h"
#include "nav.h"
#include "pwm.h"

/* Globals that define where the helicopter SHOULD be
   in terms of a percentage of the full value.  */
unsigned int yawFlag = 0;
unsigned int altFlag = 0;
unsigned int selectedFlag = 0;




/** Returns true if specified button is pressed
 	@param  pin number of specified button
 	@return 1 if button is pushed             */
unsigned int
buttonCheck (unsigned int pinNum)
{
	if (ulTempButton & (1 << pinNum))
	{
		return 0;
	}
	return 1;
}

/** Displays the relevant information with respect to the helicopter.
    @param the current altitude of the helicopter represented as a percentage.  */
void
displayStatus (unsigned int j)
{
	unsigned int temp;

	RIT128x96x4StringDraw ("STATUS      ", 5, 14, 15);

	char string2[100];
	sprintf (string2, "%.2f degrees", degrees);
	RIT128x96x4StringDraw (string2, 20, 24, 15);

	char string3[100];
	sprintf (string3, "%d", j);
	RIT128x96x4StringDraw (string3, 5, 34, 15);

	/* Checks if the percentage of the altitude is outside of the 5% - 95% range */
	if (j > 95)
	{
		temp = 95;
	}
	else if (j < 5)
	{
		temp = 5;
	}
	else
	{
		temp = j;
	}

	/* This displays the corresponding DC if desired. */
	char string4[100];
	sprintf (string4, "DC = %d", 100 - temp);
	RIT128x96x4StringDraw (string4, 6, 44, 15);

	char string5[100];
	sprintf (string5, "%d degrees", yawInput);
	RIT128x96x4StringDraw (string5, 5, 54, 15);

	char string6[100];
	sprintf (string6, "%d perc", altInput);
	RIT128x96x4StringDraw (string6, 5, 64, 15);
}


/** State machine that impliments both altitude and yaw
 	input steps depending on the buttons pressed. The
 	state machine uses globals (flags and values) that
 	get updated.
 */
void
inputStateMachine ()
{


	if (buttonCheck(4) && selectedFlag == 0)
	{
		selectedFlag = 1;

		/* If the select button is pressed, enable BOTH motors. */
		if (stateFlag == 0)
		{
			PWMOutputState (PWM_BASE, PWM_OUT_1_BIT | PWM_OUT_4_BIT, 1);
			stateFlag = 1;
		}

		else if (stateFlag == 1)
		{
			altInput -= 1;
			stateFlag = 2;
		}
	}

	else if (!buttonCheck(4))
	{
		selectedFlag = 0;
	}

	/* ALTITUDE */

	/* Checks the UP button status. */
	if (buttonCheck(5) && altFlag == 0 && stateFlag == 1)
	{
		/* If the up button has been newly pressed. */
		if (altInput <= 90)
		{
			altInput += 10;
		}
		altFlag = 1;
	}

	/* Checks the DOWN button status. */
	else if (buttonCheck(6) && altFlag == 0 && stateFlag == 1)
	{
		/* If the down button has been newly pressed. */
		if (altInput >= 10)
		{
			altInput -= 10;
		}
		altFlag = 1;
	}

	else if (!buttonCheck(5) && !buttonCheck(6) && stateFlag == 1)
	{
		/* If BOTH up & down buttons are released. */
		altFlag = 0;
	}

	/* YAW */

	/* Checks the LEFT - CCW button status. */
	if (buttonCheck(3) && yawFlag == 0 && stateFlag == 1)
	{
		/* If the left button has been newly pressed. */

		yawInput -= 15;

		yawFlag = 1;
	}
	/* Checks the RIGHT - CW button status. */
	else if (buttonCheck(2) && yawFlag == 0 && stateFlag == 1)
	{
		/* If the down button has been newly pressed. */
		yawInput += 15;
	}


	else if (!buttonCheck(3) && !buttonCheck(2) && stateFlag == 1)
	{
		/* If BOTH CW & CCW buttons are released. */
		yawFlag = 0;
	}

}


