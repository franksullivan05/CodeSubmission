/** @file 	pwm.h
	@author K. Arik & F. Sullivan
	@date 	6 May 2015
	@brief 	Support for the PWM generator.
*/


#ifndef PWM_H_
#define PWM_H_


/** Initalises a 150 Hz PWM at PWM4 (Main rotor) and initally disables the output. */
void
initPWM4 (void);


/** Initalises a 150 Hz PWM at PWM1 (Tail rotor) and initally disables the output. */
void
initPWM1 (void);


/** Sets the duty cycle for any PWM.
 	@param  the duty cycle represented as a percentage
 	@param  the PWM output wanting to be changed.
 */
void
changeDutyCycle (float dutycycle, unsigned long ulPWMOut);


#endif /*PWM_H_*/
