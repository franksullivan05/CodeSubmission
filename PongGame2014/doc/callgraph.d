system_clock_init@system.c: 

system_watchdog_timer_init@system.c: 

system_init@system.c: system_clock_init system_watchdog_timer_init

pio_config_set@pio.c: 

ledmat_init@display.c: pio_config_set pio_config_set

timer_init@timer.c: 

pacer_init@pacer.c: timer_init

timer_get@timer.c: 

timer_wait_until@timer.c: timer_get

pacer_wait@pacer.c: timer_wait_until

pio_config_get@pio.c: 

_delay_loop_1@navswitch.c: 

pio_input_get@pio.c: 

navswitch_update@navswitch.c: pio_config_get pio_config_set pio_config_set _delay_loop_1 pio_input_get pio_config_set

pio_output_high@pio.c: 

pio_output_low@pio.c: 

display_bat@paddle.c: pio_output_low pio_output_high

ball_check@ball.c: 

hit_right@ball.c: 

movement_right@ball.c: 

hit_centre@ball.c: 

hit_left@ball.c: 

movement_left@ball.c: 

pio_output_toggle@pio.c: 

ball@ball.c: ball_check hit_right movement_right hit_centre hit_left movement_left pio_output_toggle

navswitch_push_event_p@navswitch.c: 

move@control.c: navswitch_push_event_p navswitch_push_event_p

main@game.c: system_init ledmat_init pacer_init pio_config_set pacer_wait navswitch_update ledmat_init pio_output_high pio_output_high display_bat pio_output_low display_bat pio_output_low pio_output_low pio_output_high ball move

