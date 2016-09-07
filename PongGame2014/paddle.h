/**********************************************************************/
/**                                                                  **/
/** Authors:                                                         **/
/** 		K. Arik & F. Sullivan            	                     **/
/**                                                                  **/
/** Group:                                                           **/
/**         #44                                                      **/
/**                                                                  **/
/** Filename:                                                        **/
/**         paddle.h                                                 **/
/**                                                                  **/
/** Description:                                                     **/
/**	Header file that contains functions to display the paddle        **/
/** invovled in the game, aswell as a function that allows movement  **/
/** of the paddle. Also contains the bit patterns for each paddle    **/
/** position                                                         **/          
/** 											                     **/
/**                                                                  **/
/** Date of Submission:                                              **/
/**         17th October, 2014                                       **/
/**                                                                  **/
/**********************************************************************/





#ifndef PADDLE_H
#define PADDLE_H


#include "system.h"


//Bit-Patterns of all the possible bat positions the LED can contain.
static uint8_t bat_positions[] =
{
    0x70, 0x38, 0x1C, 0x0E, 0x07
};

//Moves the paddle to a new location, returning the new location of the
//paddle.
uint8_t move(uint8_t current_position);


//Displays the bat at the given bat position.
void display_bat (uint8_t row_pattern, int state);


#endif
