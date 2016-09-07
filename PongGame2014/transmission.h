/**********************************************************************/
/**                                                                  **/
/** Authors:                                                         **/
/** 		K. Arik & F. Sullivan            	                     **/
/**                                                                  **/
/** Group:                                                           **/
/**         #44                                                      **/
/**                                                                  **/
/** Filename:                                                        **/
/**         transmission.h                                           **/
/**                                                                  **/
/** Description:                                                     **/
/**			This header contains function to allow                   **/
/**         signals to be send/received across the two boards        **/
/** 		by using a series of characters where each defines a     **/
/** 		specific 'message'.        								 **/
/**                                                                  **/
/** Date of Submission:                                              **/
/**         17th October, 2014                                       **/
/**                                                                  **/
/**********************************************************************/

#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "system.h"


//Interprets the recieved information from the other board.
void received_information (char receieved_state, int *row, int *rowinc, int *colinc, int *col);


//Sends a character depending on the current state of game
char send_state (int *row, int *rowinc);


//Sends a character corresponding to a loss
char send_loss (void);

#endif
