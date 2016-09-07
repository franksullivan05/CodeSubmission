/**********************************************************************/
/**                                                                  **/
/** Authors:                                                         **/
/** 		K. Arik & F. Sullivan            	                     **/
/**                                                                  **/
/** Group:                                                           **/
/**         #44                                                      **/
/**                                                                  **/
/** Filename:                                                        **/
/**         ball.c                                                   **/
/**                                                                  **/
/** Description:                                                     **/
/**         This module describes the functionallity of the ball.    **/
/**			This includes all IR transmission and determining if the **/
/**			ball has hit the paddle or the sides of the display      **/
/**                                                                  **/
/** Date of Submission:                                              **/
/**         17th October, 2014                                       **/
/**                                                                  **/
/**********************************************************************/

#include "system.h"
#include "pio.h"
#include "paddle.h"
#include <stdlib.h>
#include "ball.h"
#include "display.h"
#include "ir_uart.h"
#include "transmission.h"
#include "led_display.h"
#include "tinygl.h"

#define SAFEGUARD 20
#define SPEED_UP 3

/** This function show the ball "pixel" dependent on the state of the multiplexing.   */
void ledmat_pixel_set (int col, int row, bool state)
{	
    if (state)
    {
		tinygl_draw_point (tinygl_point(col,row), 1);
    }
    else
    {
        tinygl_draw_point (tinygl_point(col,row), 0);
    }
}


/** Function that describes the right movement of the ball   */
void movement_right (int *rowinc, int *colinc) {
	
	if (*rowinc == 1) {
		*rowinc = 0;
	}
	
	else if (*rowinc == 0) {
		*rowinc = -1;
	}
	
	*colinc = -*colinc;	
}


/** Function that describes the left movement of the ball   */
void movement_left (int *rowinc, int *colinc) {
	
	if (*rowinc == -1) {
		*rowinc = 0;
	}
	else if (*rowinc == 0) {
		*rowinc = 1;
	}
	
	*colinc = -*colinc;
}


/** Function that determines if the ball has made contact with the 
 *  paddle on the right direction.  */
bool hit_right (int current_position, int *row) {
	
	if (((bat_positions[current_position] >> (*row + 2)) & 1) &&
		((bat_positions[current_position] >> *row) & 1)) {
			
		return 1;
	}
	
	return 0;
	
}


/** Function that determines if the ball has made contact with the 
 *  paddle on the centre of the paddle.  */
bool hit_centre (int current_position, int *row) {
	
	if (((bat_positions[current_position] >> (*row + 1)) & 1) &&
	    ((bat_positions[current_position] >> *row) & 1)) {
		
		return 1;
	}
	
	return 0;
	
}


/** Function that determines if the ball has made contact with the 
 *  paddle on the left direction.  */
 
bool hit_left (int current_position, int *row){
	
	if ((bat_positions[current_position] >> *row) & 1) {
		
		return 1;
	}
	
	return 0;
		
}



/** Function that determines if the ball has passed outside of the 
 *  bondaries.  */
void ball_check (int *row, int *rowinc, int *col, int *colinc, int *game_status) {
	
	uint8_t data;
	*col += *colinc;
	*row += *rowinc;
	
	if (*row > 6 || *row < 0) {
		
		*row -= *rowinc * 2;
		*rowinc = -*rowinc;
	}

	if (*col ==  -1) {
		data = send_state(row, rowinc);    // Send the character corresponding to the direction the ball is travelling. 
		ir_uart_putc(data);                // Send this character to the other player. 
		
	}
	
	else if (ir_uart_read_ready_p()) {
		
		data = ir_uart_getc ();
		
		if (data == 'V') {                 // Received winning character, acknowledging a win. 
			win_message_init ();           // Initialise winning message
			*game_status = 0;	           // Change the user to the information screen. 	
			
		}
		
		else {
			received_information(data , row, rowinc, colinc, col);	
		}

	}
	
}


/** Determines if the ball has missed the paddle. If it has
 * transmission of the character 'z' is sent indicated the user
 * has lost.   */

void miss (int *game_status) {
	
	uint8_t data;
	data = send_loss ();
	ir_uart_putc (data);                   // Transmit 'Z' informaing other play a loss has occured. 
	lose_message_init ();                  // Initialise the losing message
	*game_status = 0;                      // Change the game status to go to the information screen.
	

}


/** Function that controlls the movement of the ball, aswell 
 * as handles conditions on where the ball has hit the paddle/if it has missed.   */

void ball (int *col, int *row, int *colinc, int *rowinc, int current_position, int *game_status, int *incr_rate) {
	
	ball_check (row, rowinc, col, colinc, game_status);
		
	if (*col == 3 && *colinc == 1) {
		
		if (hit_right(current_position, row)) {
			// Hits right		
			if (*incr_rate >= SAFEGUARD) {             // Safe guard from increasing to a speed that is to fast
				*incr_rate = *incr_rate - SPEED_UP;    // Increase the speed of the ball if hit
				
			}
			
			movement_right (rowinc, colinc);
		}
		
		else if (hit_centre(current_position, row)) {
			// Hits center		 
			if(*incr_rate >= SAFEGUARD){               // Safe guard from increasing to a speed that is to fast
				*incr_rate = *incr_rate - SPEED_UP;    // Increase the speed of the ball if hit
				
			}
			
			*colinc = -*colinc;		
		}
		
		else if (hit_left(current_position, row)) {
			// Hits the left
			
			if (*incr_rate >= SAFEGUARD) {              // Safe guard from increasing to a speed that is to fast
				*incr_rate = *incr_rate - SPEED_UP;     // Increase the speed of the ball if hit
				
			}
			
			movement_left(rowinc, colinc);

		}	

	}
	else if (*col == 4) {
		// Missed the ball, GAME OVER!
		
		miss (game_status);
		
	}
		
}

