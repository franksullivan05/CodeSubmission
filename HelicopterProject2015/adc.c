/** @file 	adc.c
	@author K. Arik & F. Sullivan
	@date 	21 April 2015
	@brief 	Support for ADC0 conversion and storage.
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
#include "adc.h"
#include "nav.h"


/* The range the helicopter altitude can go (1V up and 2V down) */
#define ADC_RANGE 341


/** Initialise the circBuf instance.
 	@param  pointer to buffer structure
 	@param	size needed to allocate memory to
 	@return	NULL if allocation fails, else pointer to buffer data */
unsigned long *
initCircBuf (circBuf_t *buffer, unsigned int size)
{
	buffer->windex = 0;
	buffer->rindex = 0;
	buffer->size = size;
	buffer->data =
        (unsigned long *) calloc (size, sizeof(unsigned long));
	return buffer->data;
}


/** Read one value stored into the buffer at the read index & incriments by 1.
 	@param  pointer to buffer structure
 	@return	entry at the current rindex location. */
unsigned long
readCircBuf (circBuf_t *buffer)
{
	unsigned long entry;

	entry = buffer->data[buffer->rindex];
	buffer->rindex++;
	if (buffer->rindex >= buffer->size)
	{
		buffer->rindex = 0;
	}
	return entry;
}


/** Determines the average of the circular buffer, g_inBuffer.
 	@return	the average value as an integer.  */
int
circBufAvg (circBuf_t *buffer)
{
	unsigned int i;
	unsigned int sumBuffer = 0;

	for (i = 0; i <= BUF_SIZE; i++)
	{
		sumBuffer = sumBuffer + readCircBuf (buffer);
	}
	return (sumBuffer / BUF_SIZE);
}


/** Calculates the difference in error between the current position and the last. */
float
circBufDiff (circBuf_t *buffer)
{

	/* Takes the difference from one error reading to the previous. */
	return ((float) (readCircBuf (buffer) - readCircBuf (buffer)));
}


/** ADC interrupt handler where the ADC values are stored into the circular buffer. */
void
ADCIntHandler(void)
{
	unsigned long ulValue;

	/* Get the single sample from ADC0  */
	ADCSequenceDataGet (ADC0_BASE, 3, &ulValue);

	/* Place it in the circular buffer (advancing write index) */
	g_inBuffer.data[g_inBuffer.windex] = (int) ulValue;
	g_inBuffer.windex++;
	if (g_inBuffer.windex >= g_inBuffer.size)
	{
		g_inBuffer.windex = 0;
	}


	/* Clean up, clearing the interrupt */
	ADCIntClear(ADC0_BASE, 3);
}


/** Initalises ports with respect to ADC0  */
void
initADC (void)
{
	/* The ADC0 peripheral must be enabled for configuration and use. */
	SysCtlPeripheralEnable (SYSCTL_PERIPH_ADC0);


	/* Enable sample sequence 3 with a processor signal trigger  */
	ADCSequenceConfigure (ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

	/* Configure step 0 on sequence 3, Sample channel 0 */
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
						   ADC_CTL_END);

	/* Since sample sequence 3 is now configured, it must be enabled. */
	ADCSequenceEnable(ADC0_BASE, 3);

	/* Register the interrupt handler */
	ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

	/* Enable interrupts for ADC0 sequence 3 */
	ADCIntEnable(ADC0_BASE, 3);
}



/** Takes the average of the ADC0 circular buffer and determines the
 	percentage with respect to the upper and lower bounds (1V and 2V).
 	@param  the average of the ADC0 buffer.
 	@return the converted percentage reading between 1V and 2V.   */
unsigned int
altitudePercentage (unsigned int ADCReading, unsigned int upperBound)
{
	unsigned int percBuffer = 0;

	if (ADCReading >= upperBound)
	{
		// If helicopter is below starting position (> 2V)
		percBuffer = 0;
	}

	else if (ADCReading <= upperBound - ADC_RANGE)
	{
		// If helicopter is above max position (< 1V)
		percBuffer = 100;
	}
	else
	{
		// If helicopter os ANYWHERE between 1V and 2V
		percBuffer = ((upperBound - ADCReading) * 100) / ADC_RANGE;
	}

	return percBuffer;
}


