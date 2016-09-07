/** @file 	encoder.h
	@author K. Arik & F. Sullivan
	@date 	1 May 2015
	@brief 	Support for encoder reading.
*/


#ifndef ENCODER_H_
#define ENCODER_H_


#include "adc.h"


typedef struct {
	unsigned int A;		// Pin 27
	unsigned int B;		// Pin 29
} encoder_t;

encoder_t currentState;


/* Sets a global buffer showing the error values for PID control YAW. */
circBuf_t g_errorBufferYaw;


/* Number of slots in the rotary encoder. Note: each slot
   represents 4 binary petterns using pins 27 & 29. */
#define NO_OF_SLOTS 112


/* Global variable defining angle state machine */
volatile unsigned long ulTempState;
signed long yaw;
float degrees;


/** Initialises both  encoder pins (pin 27 & 29).  */
void
initEncoderPins (void);


/** Takes the first readings of the encoder pins at stores them into the
    encoder structure.    */
void
initFirstState (void);


/** Determines the new state of the encoder state machine described in lecture notes
    based on the current state and the new encoder pin readings defined in the Systick
    interrupt.  */
void
encoderStateMachine (void);


#endif /*ENCODER_H_*/
