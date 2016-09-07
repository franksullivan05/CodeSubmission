/** @file 	pwm.c
	@author K. Arik & F. Sullivan
	@date 	21 April 2015
 * 	@brief	Program to test the PWM.
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
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
#include "stdbool.h"


/** Initalises a 150 Hz PWM at PWM4 (Main rotor) and initally disables the output. */
void
initPWM4 (void)
{
	unsigned long ulPeriod;
	ulPeriod = SysCtlClockGet() / 150;

	SysCtlPeripheralEnable (SYSCTL_PERIPH_PWM);
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF);
    GPIOPinTypePWM (GPIO_PORTF_BASE, GPIO_PIN_2);
    PWMGenConfigure (PWM_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN);
    PWMGenPeriodSet (PWM_BASE, PWM_GEN_2, ulPeriod);
    PWMPulseWidthSet (PWM_BASE, PWM_OUT_4, ulPeriod * 0.50);

    /* Enable the timers, but disable the actual output. */
    PWMGenEnable (PWM_BASE, PWM_GEN_2);
    PWMOutputState (PWM_BASE, PWM_OUT_1_BIT, 0);
}


/** Initalises a 150 Hz PWM at PWM1 (Tail rotor) and initally disables the output. */
void
initPWM1 (void)
{
	unsigned long ulPeriod;
	ulPeriod = SysCtlClockGet() / 150;

	SysCtlPeripheralEnable (SYSCTL_PERIPH_PWM);
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOD);
    GPIOPinTypePWM (GPIO_PORTD_BASE, GPIO_PIN_1);
    PWMGenConfigure (PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN);
    PWMGenPeriodSet (PWM_BASE, PWM_GEN_0, ulPeriod);
    PWMPulseWidthSet (PWM_BASE, PWM_OUT_1, ulPeriod * 0.50);

    /* Enable the timers, but disable the actual output. */
    PWMGenEnable (PWM_BASE, PWM_GEN_0);
    PWMOutputState (PWM_BASE, PWM_OUT_1_BIT, 0);
}


/** Sets the duty cycle for any PWM.
 	@param  the duty cycle represented as a percentage
 	@param  the PWM output wanting to be changed.
 */
void
changeDutyCycle (float dutycycle, unsigned long ulPWMOut)
{

	/* Checks if the percentage of the altitude is outside of the 5% - 95% range */
	if (dutycycle > 95.0)
	{
		dutycycle = 95.0;
	}

	else if (dutycycle < 5.0)
	{
		dutycycle = 5.0;
	}

	/* Sets a 150 Hz PWM with the desired duty cycle. */
	unsigned long ulPeriod;
	ulPeriod = SysCtlClockGet() / 150;
	PWMPulseWidthSet (PWM_BASE, ulPWMOut, ((unsigned long) (ulPeriod * dutycycle) / 100));
}

