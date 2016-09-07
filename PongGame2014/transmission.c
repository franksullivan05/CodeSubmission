/**********************************************************************/
/**                                                                  **/
/** Authors:                                                         **/
/** 		K. Arik & F. Sullivan            	                     **/
/**                                                                  **/
/** Group:                                                           **/
/**         #44                                                      **/
/**                                                                  **/
/** Filename:                                                        **/
/**         transmission.c                                           **/
/**                                                                  **/
/** Description:                                                     **/
/**			This module deals with all signals across the two boards **/
/** 		by using a series of characters where each defines a     **/
/** 		specific 'message'.        								 **/
/**                                                                  **/
/** Date of Submission:                                              **/
/**         17th October, 2014                                       **/
/**                                                                  **/
/**********************************************************************/

#include "ir_uart.h"
#include "ball.h"
#include "system.h"
#include "transmission.h"
#include "pio.h"

#define LED_PIO PIO_DEFINE(PORT_C, 2)


/** Interprets the recieved information from the other board. */
void received_information (char receieved_state , int *row, int *rowinc, int *colinc, int* col) {
	
	//Each letter corresponds to a specific combination of row position
	//and row velocity.
	
	
	pio_output_high (LED_PIO);
	*colinc = 1;
	*col = 0;
	
	if (receieved_state == 'A') {
		*row = 6;
		*rowinc = 0;
	}
	
	else if (receieved_state == 'B') {
		*row = 6;
		*rowinc = -1;
	}
	
	else if (receieved_state == 'C') {
		*row = 6;
		*rowinc = -1;		
	}
	
	else if (receieved_state == 'D') {
		*row = 5;
		*rowinc = 0;		
	}
		
	else if (receieved_state == 'E') {
		*row = 5;
		*rowinc = -1;
	}
		
	else if (receieved_state == 'F') {
		*row = 5;
		*rowinc = 1;
	}
	
	else if (receieved_state == 'G') {
		*row = 4;
		*rowinc = 0;		
	}
	
	else if (receieved_state == 'H') {
		*row = 4;
		*rowinc = -1;		
	}
		
	else if (receieved_state == 'I') {
		*row = 4;
		*rowinc = 1;
	}
		
	else if (receieved_state == 'J') {
		*row = 3;
		*rowinc = 0;
	}
		
	else if (receieved_state == 'K') {
		*row = 3;
		*rowinc = -1;
	}
	
	else if (receieved_state == 'L') {
		*row = 3;
		*rowinc = 1;
	}
	
	else if (receieved_state == 'M') {
		*row = 2;
		*rowinc = 0;
	}
		
	else if (receieved_state == 'N') {
		*row = 2;
		*rowinc = -1;
	}
		
	else if (receieved_state == 'O') {
		*row = 2;
		*rowinc = 1;
	}
	
	else if (receieved_state == 'P') {
		*row = 1;
		*rowinc = 0;
	}
	
	else if (receieved_state == 'Q') {
		*row = 1;
		*rowinc = -1;
	}
		
	else if (receieved_state == 'R') {
		*row = 1;
		*rowinc = 1;
	}
		
	else if (receieved_state == 'S') {
		*row = 0;
		*rowinc = 0;
	}
		
	else if (receieved_state == 'T') {
		*row = 0;
		*rowinc = -1;
	}
	
	else if (receieved_state == 'U') {
		*row = 0;
		*rowinc = 1;
	}
}

/** Sends a character depending on the current state of game  */
char send_state (int *row, int *rowinc) {
	
	//Each letter corresponds to a specific combination of row position
	//and row velocity.
	
	
	pio_output_low (LED_PIO);
	char state = 0;
	
	if (*rowinc == 0) {
		
		if (*row == 0) {
			state = 'A';
		}
		
		else if(*row == 1){
			state = 'D';
		}
		
		else if (*row == 2) {
			state = 'G';
		}		
		
		else if (*row == 3) {
			state = 'J';
		}
		
		else if (*row == 4) {
			state = 'M';
		}
		
		else if (*row == 5) {
			state = 'P';
		}
		
		else if (*row == 6) {
			state = 'S';
		}
	}
	
	else if (*rowinc == 1) {
		
		if (*row == 0) {
			state = 'B';
		}
		
		else if (*row == 1) {
			state = 'E';
		}
		
		else if (*row == 2) {
			state = 'H';
		}		
		
		else if (*row == 3) {
			state = 'K';
		}
		
		else if (*row == 4) {
			state = 'N';
		}
		
		else if (*row == 5) {
			state = 'Q';
		}
		
		else if (*row == 6) {
			state = 'T';
		}
	}
		
	else if (*rowinc == -1) {
		
		if (*row == 0) {
			state = 'C';
		}
		
		else if (*row == 1) {
			state = 'F';
		}
		
		else if (*row == 2) {
			state = 'I';
		}
				
		else if (*row == 3) {
			state = 'L';			
		}
		
		else if (*row == 4) {
			state = 'O';			
		}
		
		else if (*row == 5) {
			state = 'R';			
		}
		
		else if (*row == 6) {
			state = 'U';			
		}
	}
	return state;
}

/** Function that sends a loss message if you loose. */
char send_loss (void) {
	// Sending loss
	char data = 'V';
	
	return data;
}



