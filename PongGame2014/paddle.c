/**********************************************************************/
/**                                                                  **/
/** Authors:                                                         **/
/** 		K. Arik & F. Sullivan            	                     **/
/**                                                                  **/
/** Group:                                                           **/
/**         #44                                                      **/
/**                                                                  **/
/** Filename:                                                        **/
/**         paddle.c                                                 **/
/**                                                                  **/
/** Description:                                                     **/
/**	Module that contains functionality to display the paddle invovled**/
/** in the game, aswell as a function that allows movement of the    **/
/** paddle.											                 **/
/**                                                                  **/
/** Date of Submission:                                              **/
/**         17th October, 2014                                       **/
/**                                                                  **/
/**********************************************************************/


#include "system.h"
#include "pio.h"
#include "display.h"
#include "tinygl.h"
#include "navswitch.h"


/** Function that displays the 5 possible paddle states*/

void display_bat (uint8_t row_pattern, int state)
{
	
	if (row_pattern == 0x70) {
		
		//Bit position refers to a paddle seen like so:
		// xxx....
		// x = illuminated, . = off. 
		
		tinygl_draw_point(tinygl_point(4,4),state);
		tinygl_draw_point(tinygl_point(4,5),state);  //Displays the paddle that corresponds to the bit pattern
		tinygl_draw_point(tinygl_point(4,6),state);
	}
	else if (row_pattern == 0x38) {
		
		//Bit position refers to a paddle seen like so:
		// .xxx...
		// x = illuminated, . = off. 
		
		tinygl_draw_point(tinygl_point(4,3),state);
		tinygl_draw_point(tinygl_point(4,4),state);  //Displays the paddle that corresponds to the bit pattern
		tinygl_draw_point(tinygl_point(4,5),state);
	}
	else if(row_pattern == 0x1C) {
		
		//Bit position refers to a paddle seen like so:
		// ..xxx..
		// x = illuminated, . = off.
		
		tinygl_draw_point(tinygl_point(4,2),state);
		tinygl_draw_point(tinygl_point(4,3),state);  //Displays the paddle that corresponds to the bit pattern
		tinygl_draw_point(tinygl_point(4,4),state);
	}
	else if (row_pattern == 0x0E) {
		
		//Bit position refers to a paddle seen like so:
		// ...xxx.
		// x = illuminated, . = off.
		
		tinygl_draw_point(tinygl_point(4,1),state);
		tinygl_draw_point(tinygl_point(4,2),state);  //Displays the paddle that corresponds to the bit pattern
		tinygl_draw_point(tinygl_point(4,3),state);
	}
	else if (row_pattern == 0x07) {
		
		//Bit position refers to a paddle seen like so:
		// ....xxx
		// x = illuminated, . = off.
		
		tinygl_draw_point(tinygl_point(4,0),state);
		tinygl_draw_point(tinygl_point(4,1),state);  //Displays the paddle that corresponds to the bit pattern
		tinygl_draw_point(tinygl_point(4,2),state);
	}
	
	

}



/** Function that controlls the movement of the paddle.  */

uint8_t move(uint8_t current_position){
	
		if ((navswitch_push_event_p (NAVSWITCH_NORTH)) && (current_position < 4)) {
			current_position++; //Move Right
			
		}
		
		else if ((navswitch_push_event_p (NAVSWITCH_SOUTH)) && (current_position > 0)) {
			current_position--; //Move Left
			
		}
		navswitch_update (); //Update Navswitch
		
		
		return current_position;
	
	
}
