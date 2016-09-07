/**********************************************************************/
/**                                                                  **/
/** Authors:                                                         **/
/** 		K. Arik & F. Sullivan            	                     **/
/**                                                                  **/
/** Group:                                                           **/
/**         #44                                                      **/
/**                                                                  **/
/** Filename:                                                        **/
/**         game.c                                                   **/
/**                                                                  **/
/** Description:                                                     **/
/**         This is a simple game of pong played over two ucfk4. The **/
/**         ball is transmitted over infrared when in gets to the    **/
/**         end of one screen. As play goes on, the speed of the     **/
/**         ball increases to a certain level. When the ball is      **/
/** 		missed by either player, a message is shown to say who   **/
/**         wins and who looses. The navswitch button is pressed     **/
/**         again to re-initialise the entire game as well as the    **/
/**         speed. The player who presses the button will start with **/
/**			the ball.												 **/
/**                                                                  **/
/** Date of Submission:                                              **/
/**         17th October, 2014                                       **/
/**                                                                  **/
/**********************************************************************/

#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#include "paddle.h"
#include "ball.h"
#include "led_display.h"
#include "tinygl.h"
#include "ir_uart.h"

/* Define polling rate in Hz.  */
#define LOOP_RATE 200

/* Define blue LED  */
#define LED_PIO PIO_DEFINE(PORT_C, 2)


/** Function that displays information to the user while the game is not
 * running. Allows the user to also trasmit into a state of playing by
 * pushing down on the Navswitch.    */
 
static void game_restart(int *incr_rate, ball_state *current_game, 
						 int *game_status, int *current_position){
		
		
	tinygl_update();  
	navswitch_update ();
		
		
	
		
	if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
		
		/* If the push button is pressed sets up the game with 
		 * the play who pushed the button starting with the ball. 
		 * Transmits the character 'W' to the other chip to notify
		 * the other player the game has started. 
													   */
		pio_output_high (LED_PIO);
		*incr_rate = 50; 
		(*current_game).row = 3;
		(*current_game).col = 0;
		(*current_game).rowinc = 0;
		(*current_game).colinc = 1;
		*current_position = 2;
		*game_status = 1;
		ir_uart_putc('W');
		

	}
		
		
	else if(ir_uart_read_ready_p()){
		
		/* If the other play has pushed down on the Navswitch, will
		 * recieve information to now put the player into the state
		 * of playing. 
													   */
		if (ir_uart_getc() == 'W') {
			pio_output_low (LED_PIO);
			*incr_rate = 50;
			(*current_game).row = 3;
			(*current_game).col = -2;
			(*current_game).rowinc = 0;
			(*current_game).colinc = 0;
			*current_position = 2;
			*game_status = 1;
		}

	}

	
}


static void ball_move(ball_state *current_game, int *game_status, int*  
			incr_rate, int* current_position, int *slow_rate){
	
	ball (&((*current_game).col), 
	  &((*current_game).row), 
	  &((*current_game).colinc), 
	  &((*current_game).rowinc), 
	  *current_position, 
	  game_status, 
	  incr_rate);

	*slow_rate = 0;
	
}




int main (void)
{
	
	
	//-------------------------INITIALISER----------------------------//
	
	int game_status = 0; //determines if in game mode, or information
			//mode
	int incr_rate = 50; // determines the speed of the ball
    int slow_rate = 0; // deterimines the speed of the ball
    int current_position = 2; //Previous position of the bat
    static int row_prev = 0; //Previous row position of the ball
    static int col_prev = 0; //Previous col position of the ball
    static int prev_position = 0;
    uint8_t flag = 0; 
    ball_state current_game = {3,0,0,1}; //co-ordinates/velocity of the ball
    
	system_init ();
	pacer_init (LOOP_RATE);	
    pio_config_set(LED_PIO, PIO_OUTPUT_LOW);
	welcome_message_init(); //introduces the game with a welcome message
    navswitch_init ();
    ir_uart_init();

    //------------------------------------------------------------------
    
    while (1)
    {
		if (game_status == 0) {
			
			if(flag == 1){
				flag = 0;
				
			}
			
			//Displays information to the user. 
			
			game_restart(&incr_rate, &current_game, 
						 &game_status, &current_position);
	
			}
				
		else {
			
			if(flag == 0){
				tinygl_clear(); //Clears the message from information
				//state
				flag = 1;
			}
			//-----------------MULTIPLEX BETWEEN BALL AND BAT----------
			
			ledmat_pixel_set (col_prev, row_prev, 0);
			ledmat_pixel_set (current_game.col, current_game.row, 1);
			display_bat (bat_positions[prev_position], 0);
			display_bat (bat_positions[current_position], 1);
			row_prev = current_game.row;
			col_prev = current_game.col;
			prev_position = current_position;
			
			//---------------------------------------------------------
			
			if (slow_rate >= incr_rate) {
				//Moves the ball,		
				ball_move(&current_game, &game_status, &incr_rate, 
										 &current_position, &slow_rate);
				
			}
			//Determines the position of the bat. 
			current_position = move(current_position);
			slow_rate++; 

			
			tinygl_update();
		}
		
		pacer_wait ();
		
    }
	return 0;
}



