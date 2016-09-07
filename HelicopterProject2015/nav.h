/** @file 	nav.h
	@author K. Arik & F. Sullivan
	@date 	21 April 2015
	@brief 	Support for navagation buttons with debouncing.
*/


#ifndef NAV_H_
#define NAV_H_


/* RELEASED_MAX represents 20ms with respect to the systick interrupts. */
#define PRESSED_MAX 4
#define RELEASED_MAX 13

/* Button structure to hold states, pins and ports */
typedef struct {
	int pressed;					// Pressed state  (1 or 0)
	int released;					// Released state (1 or 0)
	int uiPressLevel;				// Counter for PRESSED debouncing delay
	int uiReleasedLevel;			// Counter for RELEASED debouncing delay
	unsigned long ulPort;			// Base argument for GPIO port
	unsigned int ucPin;				// Pin number for GPIO port
} button_t[5];

/* Holds the readings of the button pins (global)   */
volatile unsigned long ulTempButton;
volatile unsigned long ulTempButtonNEW;

/* Flag that determines the select button has implimented.
   stateFlag = 0 -> MOTORS OFF
   stateFlag = 1 -> NORMAL OPERATION
   stateFlag = 2 -> LANDING SEQUENCE    */
int stateFlag;



/* Globals that define where the helicopter SHOULD be
   in terms of a percentage of the full value.  */
int yawInput;
int altInput;


static button_t butArray = {{0, 0, 0, 0, GPIO_PORTB_BASE, 5},  // UP
					 	 	{0, 0, 0, 0, GPIO_PORTB_BASE, 6},  // DOWN
							{0, 0, 0, 0, GPIO_PORTB_BASE, 3},  // LEFT 	-- CCW
							{0, 0, 0, 0, GPIO_PORTB_BASE, 2},  // RIGHT	-- CW
							{0, 0, 0, 0, GPIO_PORTB_BASE, 4}}; // SELECT


/** Returns true if specified button is pressed
 	@param  pin number of specified button
 	@return 1 if button is pushed             */
unsigned int
buttonCheck (unsigned int pinNum);


/** Displays the relevant information with respect to the helicopter.
    @param the current altitude of the helicopter represented as a percentage.  */
void
displayStatus (unsigned int j);


/** State machine that impliments both altitude and yaw
 	input steps depending on the buttons pressed. The
 	state machine uses globals (flags and values) that
 	get updated.
 */
void
inputStateMachine ();



#endif /*ADC_H_*/
