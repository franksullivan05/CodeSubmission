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

#ifndef BALL_H
#define BALL_H

#include "system.h"

typedef struct ball_information ball_state; 



//Contains the information reguarding the ball.
struct ball_information{
	int row;
	int col;
	int rowinc;
	int colinc;
};


//Shows the ball pixle.
void ledmat_pixel_set (int col, int row, bool state);


//Controlls movement of the ball.
void ball (int *col, int *row, int *colinc, int *rowlinc, int current_position, int* game_status, int* incr_rate);

#endif
