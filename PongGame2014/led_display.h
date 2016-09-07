/**********************************************************************/
/**                                                                  **/
/** Authors:                                                         **/
/** 		K. Arik & F. Sullivan            	                     **/
/**                                                                  **/
/** Group:                                                           **/
/**         #44                                                      **/
/**                                                                  **/
/** Filename:                                                        **/
/**         led_display.h                                            **/
/**                                                                  **/
/** Description:                                                     **/
/**	Header that contains function to initialised the information     **/ 
/** that is being displayed to the user, aswell as the entire grid in**/
/** general grid in general.                                         **/ 
/** 									                             **/
/**                                                                  **/
/** Date of Submission:                                              **/
/**         17th October, 2014                                       **/
/**                                                                  **/
/**********************************************************************/


#ifndef LED_DISPLAY_H
#define LED_DISPLAY_H

#include "system.h"


/** Define PIO pins driving LED matrix rows.  */
static const pio_t rows[] =
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO, 
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};


/** Define PIO pins driving LED matrix columns.  */
static const pio_t cols[] =
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};

//Initialise the entire led grid.
void ledmat_initi (void);

//Initialise welcome message.
void welcome_message_init(void);


//Initialise win message.
void win_message_init(void);


//Initialise lose message.
void lose_message_init(void);


#endif
