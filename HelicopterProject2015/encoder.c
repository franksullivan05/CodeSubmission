/** @file 	encoder.c
	@author K. Arik & F. Sullivan
	@date 	1 May 2015
	@brief 	Support for encoder reading.
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
#include "stdio.h"
#include "stdlib.h"
#include "encoder.h"
#include "nav.h"
#include "adc.h"


/** Initialises both  encoder pins (pin 27 & 29).  */
void
initEncoderPins (void)
{
	/* Initialising both pin 27 & 29 */
	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOInput (GPIO_PORTF_BASE, GPIO_PIN_5 | GPIO_PIN_7);
}


/** Takes the first readings of the encoder pins at stores them into the
    encoder structure.    */
void
initFirstState (void)
{
	unsigned long initalStatus;
	initalStatus = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_5 | GPIO_PIN_7);
	currentState.A = 1 & (initalStatus >> 5);
	currentState.B = 1 & (initalStatus >> 7);
	yaw = 0;
}


/** Determines the new state of the encoder state machine described in lecture notes
    based on the current state and the new encoder pin readings defined in the Systick
    interrupt.  */
void
encoderStateMachine (void)
{
	if (currentState.A == 0 && currentState.B == 0)
	{
		/* If state 1 */

		if (1 & (ulTempState >> 7))
		{
			/* If B has changed to 1 */

			yaw++;
			currentState.B = 1;
		}
		else if (1 & (ulTempState >> 5))
		{
			/* If B has changed to 1 */

			yaw--;
			currentState.A = 1;
		}
	}

	else if (currentState.A == 0 && currentState.B == 1)
	{
		/* If state 2 */

		if (1 & (ulTempState >> 5))
		{
			/* If A has changed to 1 */

			yaw++;
			currentState.A = 1;
		}
		else if (!(1 & (ulTempState >> 7)))
		{
			/* If B has changed to 0 */

			yaw--;
			currentState.B = 0;
		}
	}

	else if (currentState.A == 1 && currentState.B == 1)
	{
		/* If state 3 */

		if (!(1 & (ulTempState >> 7)))
		{
			/* If B has changed to 0 */

			yaw++;
			currentState.B = 0;
		}
		else if (!(1 & (ulTempState >> 5)))
		{
			/* If A has changed to 0 */

			yaw--;
			currentState.A = 0;
		}
	}

	else if (currentState.A == 1 && currentState.B == 0)
	{
		/* If state 4 */

		if (!(1 & (ulTempState >> 5)))
		{
			/* If A has changed to 0 */

			yaw++;
			currentState.A = 0;
		}
		else if (1 & (ulTempState >> 7))
		{
			/* If B has changed to 1 */

			yaw--;
			currentState.B = 1;
		}
	}
}
