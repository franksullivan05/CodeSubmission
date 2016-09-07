/**********************************************************************/
/**                                                                  **/
/** Authors:                                                         **/
/** 		K. Arik & F. Sullivan            	                     **/
/**                                                                  **/
/** Group:                                                           **/
/**         #44                                                      **/
/**                                                                  **/
/** Filename:                                                        **/
/**         led_display.c                                            **/
/**                                                                  **/
/** Description:                                                     **/
/**	Module that contains functionality to initialised the information**/ 
/** that is being displayed to the user, aswell as the entire grid in**/
/** general grid in general.                                         **/ 
/** 									                             **/
/**                                                                  **/
/** Date of Submission:                                              **/
/**         17th October, 2014                                       **/
/**                                                                  **/
/**********************************************************************/




#include "system.h"
#include "pio.h"
#include "led_display.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"



//Speed at the message is presented to the user. 
#define MESSAGE_RATE 15

/* Define polling rate in Hz.  */
#define LOOP_RATE 200




/** Initialise LED matrix PIO pins.  */
void ledmat_initi (void)
{	
	int row = 0;
	int col = 0;

    for (row = 0 ; row < 7; row++)
    {
        pio_config_set (rows[row], PIO_OUTPUT_HIGH); //Initialising all rows

    }

    for (col = 0 ; col < 5; col++)
    {
        pio_config_set (cols[col], PIO_OUTPUT_HIGH); //Initialising all coloumns

    }
};



/** Initialise welcome message*/
void welcome_message_init(void){
	
	tinygl_init (LOOP_RATE);
	tinygl_font_set (&font3x5_1);
	tinygl_text_speed_set (MESSAGE_RATE);
	tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
	tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
	tinygl_text("WELCOME!!!");
	
	
	
}

/** Initialise winning message*/
void win_message_init(void){
	tinygl_clear();
	tinygl_init (LOOP_RATE);
	tinygl_font_set (&font3x5_1);
	tinygl_text_speed_set (MESSAGE_RATE);
	tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
	tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
	tinygl_text("YOU WIN");
	
	
}


/** Initialise losing message*/
void lose_message_init(void){
	
	tinygl_clear(); 
	tinygl_init (LOOP_RATE);
	tinygl_font_set (&font3x5_1);
	tinygl_text_speed_set (MESSAGE_RATE);
	tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
	tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
	tinygl_text("YOU LOSE");
	
	
}



