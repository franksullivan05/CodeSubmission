/** @file 	final.c
	@author K. Arik & F. Sullivan
	@date 	13 May 2015
 	@brief	Final program for the assesment of the Helocopter Project
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "driverlib/debug.h"
#include "drivers/rit128x96x4.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "stdio.h"
#include "encoder.h"
#include "adc.h"
#include "nav.h"
#include "pwm.h"


#define TICK_RATE_HZ 5000ul


/* Current ADC reading (averaged) converted to the percentage of the helicopter altitude range. */
int percentage;



void
SysTickIntHandler(void)
{
    /* Initiate a conversion.  */
    ADCProcessorTrigger(ADC0_BASE, 3);

	/* At each sysTick, the pins 27 & 29 are read and stored into a temporary value */
	ulTempState = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_7 | GPIO_PIN_5);

	/* Calculates the angle in degrees from the global yaw variable. */
	degrees = ((float) yaw * 360) / (4* NO_OF_SLOTS);


	/* Updates the global variable, yaw.  */
	encoderStateMachine ();
}


void
PinChangeIntHandler (void)
{
	/* Reads in the new pin values of all buttons. */
	ulTempButtonNEW = GPIOPinRead (GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

	/* Clears the pin interrupt that initiated the pin change interrupt by the use of old XOR new. */
	GPIOPinIntClear (GPIO_PORTB_BASE, ulTempButton ^ ulTempButtonNEW);

	/* Sets the new reading equal to the old value. */
	ulTempButton = ulTempButtonNEW;

	/* Updates the desired helicopter levels based on the buttons pressed. */
	inputStateMachine ();
}


/** Initialisation of all pins. */
void
initPin (void)
{
	/* Initialises entire port, including LED's */
	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);

	/* Initialisation of the pin change interrupt on the button port. */
	GPIOPortIntRegister (GPIO_PORTB_BASE, PinChangeIntHandler);

	/* Defines the drive current of each button. */
	GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	/* Set up the pin change interrupt for both edges. */
	GPIOIntTypeSet (GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_BOTH_EDGES);

	/* Enable the pin change interrupt. */
	GPIOPinIntEnable (GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
}



void
initClock (void)
{
	/* Set the clock rate. */
	SysCtlClockSet (SYSCTL_SYSDIV_22 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
}

void
initSysTick (void)
{
	/* Set up the period for the SysTick timer. */
	SysTickPeriodSet (SysCtlClockGet() / TICK_RATE_HZ);

	/* Register the interrupt handler. */
	SysTickIntRegister (SysTickIntHandler);

	/* Enable interrupt and device. */
	SysTickIntEnable ();
	SysTickEnable ();
}


void
initDisplay (void)
{
	/* intialise the OLED display. */
	RIT128x96x4Init(1000000);
}


/** Initialisation function to provide a Vcc source on Pin 56. */
void
initRefPin (void)
{
   // To set Pin 56 (PD0) as a +Vcc low current capacity source:
   SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOD);
   GPIOPinTypeGPIOOutput (GPIO_PORTD_BASE, GPIO_PIN_0);
   GPIOPadConfigSet (GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA,
		   	   	   	 GPIO_PIN_TYPE_STD_WPU);
   GPIOPinWrite (GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PIN_0);
}


/** Determines the landing sequence of the heli. Once 'select' is
    pressed, the flag is set and other buttons are ingnored.
    @param  Takes in the current loop count.
    @return Returns the new loop count.
 */
int
engageLandingSeq (int counter)
{
	/* IF the landing sequence is initiated and the loop count has reached its maximum. */
	if (percentage > 10 && stateFlag == 2 && counter == 8)
	{
		altInput -= 1;
		counter = 0;
	}

	else if (percentage <= 8 && stateFlag == 2)
	{
		PWMOutputState (PWM_BASE, PWM_OUT_1_BIT | PWM_OUT_4_BIT, 0);
		stateFlag = 0;
		counter = 0;
	}

	return counter;
}



int
main(void)
{
	initClock ();
	stateFlag = 0;
	initRefPin();
	initPin ();
	initADC ();
	initEncoderPins ();
	initPWM1 ();
	initPWM4 ();
	initDisplay ();
	initSysTick ();


	/* Determine the initial state of the yaw state machine */
	initFirstState ();

	/* Initialise ADC and error circular buffers. */
	initCircBuf (&g_inBuffer, BUF_SIZE);
	initCircBuf (&g_errorBufferAlt, BUF_SIZE);
	initCircBuf (&g_errorBufferYaw, BUF_SIZE);


	/* PID Gains for ALTITUDE. */
	float KpAlt = 0.65;
	float KiAlt = 0.001;
	float KdAlt = 0.7;

	/* PID Gains for YAW. */
	float KpYaw = 0.45;
	float KiYaw = 0.0009;
	float KdYaw = 1.4;


	/* Integral errors. */
	float integralAlt = 0;
	float integralYaw = 0;

	float yawErrorDiff;
	float altErrorDiff;

	/* Temporary Duty Cycle calculations. */
	float DCTempAlt;
	float DCTempYaw;

	/* Derivative errors. */
	float derivativeAlt;
	float derivativeYaw;
	float previousErrorAlt = 0;
	float previousErrorYaw = 0;

	/* The startup PWM driving DC's for main and tail rotor. */
	yawInput = 0;
	altInput = 0;

	/* The check flag for the first time of ADC0 reading */
	unsigned int CALIBRATION_FLAG = 1;


	int loopCount = 0;

    /* Enable interrupts to the processor. */
    IntMasterEnable ();

	while (1)
	{

		/* CRITICAL SECTION - avoids data sharing problem! */
		IntMasterDisable ();

		/* Calculate the mean of buffer and convert to a percentage. */
		percentage = altitudePercentage (circBufAvg (&g_inBuffer), ADC_UPPER_BOUND);

		IntMasterEnable ();

		/* For Differential Control. */
		yawErrorDiff = circBufDiff (&g_errorBufferYaw);
		altErrorDiff = circBufDiff (&g_errorBufferAlt);

		displayStatus (percentage);

		IntMasterDisable ();

		/* Derivative error using change between current and previous values. The
		   previous value is then set as the current value (for next loop). */
		derivativeAlt = (altInput - percentage) - previousErrorAlt;
		previousErrorAlt = (altInput - percentage);
		derivativeYaw = (yawInput - degrees) - previousErrorYaw;
		previousErrorYaw = (yawInput - degrees);

		/* Integral error is equal to the summation of errors for ALL time. */
		integralAlt = integralAlt + (altInput - percentage);
		integralYaw = integralYaw + (yawInput - degrees);


		/* Determines the dutycycle of main and tail rotors based on PID calculation above. */
		DCTempAlt = (KpAlt * (altInput - percentage)) +
					(KiAlt * integralAlt) +
				    (KdAlt * derivativeAlt);

		DCTempYaw = (KpYaw * (yawInput - degrees)) +
					(KiYaw * integralYaw) +
                    (KdYaw * derivativeYaw);

		/* Determines the desired input levels based on the landing sequence. */
		/* IF the landing sequence is initiated and the loop count has reached its maximum. */
		if (percentage > 10 && stateFlag == 2 && loopCount == 8)
		{
			altInput -= 1;

		}

		else if (percentage <= 8 && stateFlag == 2)
		{
			PWMOutputState (PWM_BASE, PWM_OUT_1_BIT | PWM_OUT_4_BIT, 0);
			stateFlag = 0;
			loopCount = 0;
		}

		else if (loopCount == 9)
		{
			loopCount = 0;

		}

		IntMasterEnable ();


		/* The altitude input controls the DC of PWM1 (main rotor). */
		changeDutyCycle ((DCTempAlt), PWM_OUT_1);

		/* The yaw input controls the DC of PWM4 (tail rotor). */
		changeDutyCycle ((DCTempYaw), PWM_OUT_4);


		/* Calibration Process happens after 1 loop. */
		if (CALIBRATION_FLAG)
		{
			/* Sets the lower and upper bounds of the helicopter between 1V and 2V */
			ADC_UPPER_BOUND = circBufAvg (&g_inBuffer);
			CALIBRATION_FLAG = 0;
		}

		loopCount++;
	}

}
