/** @file 	adc.h
	@author K. Arik & F. Sullivan
	@date 	21 April 2015
	@brief 	Support for ADC0 conversion and storage.
*/


#ifndef ADC_H_
#define ADC_H_

#define BUF_SIZE 20

/* The upper bound represents approximately 2V (when the helicopter is down) */
int ADC_UPPER_BOUND;

/* Circular Buffer structure */
typedef struct {
	unsigned int size;		// Number of entries in buffer
	unsigned int windex;	// index for writing, mod(size)
	unsigned int rindex;	// index for reading, mod(size)
	unsigned long *data;	// pointer to the data
} circBuf_t;

/* Sets a GLOBAL buffer for all to see and write */
circBuf_t g_inBuffer;

/* Sets a global buffer showing the error values for PID control. */
circBuf_t g_errorBufferAlt;


/** Initialise the circBuf instance.
 	@param  pointer to buffer structure
 	@param	size needed to allocate memory to
 	@return	NULL if allocation fails, else pointer to buffer */
unsigned long *
initCircBuf (circBuf_t *buffer, unsigned int size);


/** Read one value stored into the buffer at the read index & incriments by 1.
 	@param  pointer to buffer structure
 	@return	entry at the current rindex location. */
unsigned long
readCircBuf (circBuf_t *buffer);


/** Determines the average of ANY circular buffer, g_inBuffer.
 	@return	the average value as an integer.  */
int
circBufAvg (circBuf_t *buffer);


/** Calculates the difference in error between the current position and the last. */
float
circBufDiff (circBuf_t *buffer);


/** ADC interrupt handler where the ADC values are stored into the circular buffer. */
void
ADCIntHandler(void);


/** Initalises ports with respect to ADC0  */
void
initADC (void);


/** Takes the average of the ADC0 circular buffer and determines the
 	percentage with respect to the upper and lower bounds (1V and 2V).
 	@param  the average of the ADC0 buffer.
 	@return the converted percentage reading between 1V and 2V.   */
unsigned int
altitudePercentage (unsigned int ADCReading, unsigned int lowerBound);


#endif /*ADC_H_*/
