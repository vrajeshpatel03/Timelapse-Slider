//*************************************************************************************
/** \file intervelometer.cc
*	
*  Revisions:
*	\li	01/19/13	VP	

*
*  License:
*	This file released under the Lesser GNU Public License, version 2. This program
*	is intended for educational use only, but its use is not limited thereto. 
*/
//*************************************************************************************

#include <stdlib.h>			// Standard C library
#include <avr/io.h>			// AVR IO library
#include <avr/interrupt.h>	// Interrupt handling functions
#include "rs232.h"			// Serial Port library
#include "stl_timer.h"		// Timer library
#include "stl_task.h"		// Task Timer Library
#include "intervelometer.h"	//intervelometer motor h file include


#define PWM_FREQ OCR5A


//-------------------------------------------------------------------------------------
/** This function reads 16bit values from 16 bit register called PWM_FREQ. Set the
 * 	PWM_FREQ in the pre-processor directive at the beginning of stepper.cc file.
 *  @return returns the 16bit value from PWM_FREQ register
 */
uint16_t intervelometer::read_16bit()
{
	uint8_t sreg;		//8 bit variable to store global interrupt flag
	uint16_t val;		//16 bit value to return after reading
	
	sreg = SREG;		//store current global interrupt flag
	cli();				//disable interrupts
	val = PWM_FREQ;		//read the PWM value and store it in variable val
	SREG = sreg;		//Restore global interrupt flag
	return val;			//return answer
	
}

//-------------------------------------------------------------------------------------
/** This function writes the 16bit value to 16bit register called PWM_FREQ. Set the
 * 	PWM_FREQ in the pre-processor directive at the beginning of stepper.cc file.
 *  @param var 16bit variables to write to PWM_FREQ register
 */
void intervelometer::write_16bit(uint16_t var)
{
	uint8_t sreg;		//8bit variable to store global interrupt flag
	
	sreg=SREG;			//save current interrupt flag
	cli();				//disable interrupts
	PWM_FREQ = var;		//set the 16bit value into register
	SREG = sreg;		//restore global interrupts flag
	
}


//Constructor
//-------------------------------------------------------------------------------------
/** This is the constructor of intervelometer class. Here the pwm_setup() method, step_mode() methods are called
 *	to setup the intervelometer motor. 
 *  @param 
 *  @return
 */
intervelometer::intervelometer(time_stamp* p_stamp, base_text_serial* p_ser, task_timer* p_time)
{
	p_time_stamp = p_stamp;			// copy time_stamp pointer
	p_serial = p_ser;				// copy serial pointer
	p_timer = p_time;				// copy task timer pointer
	pwm_setup();
	
}

void intervelometer::SetTimelapse(unsigned int s_speed)
{
	shutter_speed = s_speed;	// copy shutter speed 
	//pwm_setup(); 				// setup pwm stuff
}

void intervelometer::SetPicDelay(unsigned int s_speed)
{
	shutter_speed = s_speed;	// copy shutter speed 
	//pwm_setup(); 				// setup pwm stuff
}

void intervelometer::SetMotorDelay(unsigned int s_speed)
{
	shutter_speed = s_speed;	// copy shutter speed 
	//pwm_setup(); 				// setup pwm stuff
}


void intervelometer::pwm_setup()
{
	
	//disable interrupt while setting up timer.
	//cli();
	
	DDRL |= (1<<DDL5);
	
	//set shutter speed to 5 seconds for test
	//shutter_speed = 5;
	
	//start timer, CTC, 256 pre-scalar
	//TCCR5B |= (1<<CS52) | (1<<WGM52);
	
	//clear the counter
	TCNT5 = 0;
	
	//set up frequency to 1Hz = 1 second
	write_16bit(62499);
	
	//timer/counter 5 interrupt control  
	TIMSK5 |= (1<<OCIE5C);
	
	//enable interrupt for pwm signal
	//sei();

	*p_serial <<endl <<"Intervelometer CTC setup complete!!";

}

void intervelometer::stop_timer()
{
	//toggle pin LOW
	PORTL &= ~(1<<PORTL5);
	
	//turn off timer by setting pre-scalar 0
	TCCR5B &= ~(1<<CS50) | ~(1<<CS51) | ~(1<<CS52);
	
	//reset shutter compare variable
	shutter_compare = 0;
}

void intervelometer::take_pic()
{
	//start timer, CTC, 256 pre-scalar
	TCCR5B |= (1<<CS52);
	TCCR5B |= (1<<WGM52);
	
	//pwm_setup();
	
	//set L5 to HIGH
	PORTL |= (1<<PORTL5);
	
	//clear the counter
	TCNT5 = 0;
}

void intervelometer::delay_loop()
{
	//start timer, CTC, 256 pre-scalar
	TCCR5B |= (1<<CS52);
	TCCR5B |= (1<<WGM52);
	
	//clear the counter
	TCNT5 = 0;
}

// following line turns on automatic (because I am lazy, or smart, there is a fine line) indentation for Kate editor.
// kate: space-indent on; indent-width 5; mixedindent off; indent-mode cstyle;