//*************************************************************************************
/** \file stepper.cc
*	
*  Revisions:
*	\li	01/19/13	VP	
*	\li	11/01/13	VP	Modified for Big Easy Driver
*
*  License:
*	This file released under the Lesser GNU Public License, version 2. This program
*	is intended for educational use only, but its use is not limited thereto. 
*/
//*************************************************************************************

#include <stdlib.h>			// Standard C library
#include <avr/io.h>			// AVR IO library
#include <avr/interrupt.h>	// Interrupt handling functions
#include "rs232.h"			// RS232 Library
#include "stl_timer.h"		// timer library
#include "stl_task.h"		// task library  -- dont think you need this here..
#include "stepper.h"		//stepper motor h file include

//Pin assignment for stepper motor pwm and direction
#define PWM_FREQ     	OCR4A		//variable to change pwm amount of
#define PRESCALER		256			//prescaller amount

#define DIR_PIN_DDR		DDRG		//Direction pin data direction register
#define DIR_BIT			DDG0		//direction bitmask for direction register
#define DIR_PORT		PORTG		//direction PORT register
#define DIR_PORT_BIT	PORTG0		//direction port pin

#define MS1_PIN_DDR		DDRG		//Direction pin data direction register
#define MS1_BIT			DDG2		//direction bitmask for direction register
#define MS1_PORT		PORTG		//direction PORT register
#define MS1_PORT_BIT	PORTG2		//direction port pin

#define MS2_PIN_DDR		DDRG		//Direction pin data direction register
#define MS2_BIT			DDG1		//direction bitmask for direction register
#define MS2_PORT		PORTG		//direction PORT register
#define MS2_PORT_BIT	PORTG1		//direction port pin

#define MS3_PIN_DDR		DDRD		//Direction pin data direction register
#define MS3_BIT			DDD7		//direction bitmask for direction register
#define MS3_PORT		PORTD		//direction PORT register
#define MS3_PORT_BIT	PORTD7		//direction port pin

#define LSTOP_SENSOR_DDR		DDRC		//Data Direction Register for Left stop sensor
#define LSTOP_SENSOR_DDR_BIT	DDC5		//Bit for Data direction register for left stop sensor
#define LSTOP_SENSOR_PIN		PINC		//Pin for reading state of the left stop sensor
#define LSTOP_SENSOR_PIN_BIT	PINC5		//pin register for reading state of the left stop sensor


//-------------------------------------------------------------------------------------
/** This function reads 16bit values from 16 bit register called PWM_FREQ. Set the
 * 	PWM_FREQ in the pre-processor directive at the beginning of stepper.cc file.
 *  @return returns the 16bit value from PWM_FREQ register
 */
uint16_t stepper::read_16bit()
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
void stepper::write_16bit(uint16_t var)
{
	uint8_t sreg;		//8bit variable to store global interrupt flag
	
	sreg=SREG;			//save current interrupt flag
	cli();				//disable interrupts
	PWM_FREQ = var;		//set the 16bit value into register
	SREG = sreg;		//restore global interrupts flag
	
}

//-------------------------------------------------------------------------------------
/** This method can be called to change stepping mode. EasyDriver has four stepping modes:
 * Full Step, Half Step, Quarter Step and Eighth Step. Note that anything other than Full step
 * will reduce the total torque produced by stepper motor. However, you can use finer steps to
 * produce small movements. This is for BigEasyDriver but it can also be used for Easy Stepper
 * if MS3 pin is grounded on arduino. 
 *  @param	mode	A mode to set to. Valid values are 1,2,3,4,5.
				1 = Full Step
				2 = Half Step
				3 = Quarter Step
				4 = Eighth Step
				5 = Sixteenth Step
				Defaults to Full Step
 */
void stepper::step_mode(unsigned char mode)
{
	//Setup pins as output by writing HIGH
	MS1_PIN_DDR |= (1<<MS1_BIT);
	MS2_PIN_DDR |= (1<<MS2_BIT);
	MS3_PIN_DDR |= (1<<MS3_BIT);
	
	//Mode 1: Full Step, 2Phase, (MS1 = low) (MS2 = low) (MS3 = Low)
	if (mode == 1) 
	{
		MS1_PORT &= ~(1<<MS1_PORT_BIT);	//write LOW
		MS2_PORT &= ~(1<<MS2_PORT_BIT);	//Write LOW
		MS3_PORT &= ~(1<<MS3_PORT_BIT);	//Write LOW
	}
	
	//Mode 2: Half Step, (MS1 = high) (MS2 = low) (MS3 = low)
	else if (mode == 2)
	{
		MS1_PORT |= (1<<MS1_PORT_BIT);	//write HIGH
		MS2_PORT &= ~(1<<MS2_PORT_BIT);	//write LOW
		MS3_PORT &= ~(1<<MS3_PORT_BIT);	//Write LOW
	}
	
	//Mode 3: Quarter Step, (MS1 = low) (MS2 = high) (MS3 = low)
	else if (mode == 3)
	{
		MS1_PORT &= ~(1<<MS1_PORT_BIT);	//write LOW
		MS2_PORT |= (1<<MS2_PORT_BIT);	//write HIGH
		MS3_PORT &= ~(1<<MS3_PORT_BIT);	//Write LOW
	}
	
	//Mode 4: Eighth Step, (MS1 = high) (MS2 = high) (MS3 = low)
	else if (mode == 4)
	{
		MS1_PORT |= (1<<MS1_PORT_BIT);	//write HIGH
		MS2_PORT |= (1<<MS2_PORT_BIT);	//write HIGH
		MS3_PORT &= ~(1<<MS3_PORT_BIT);	//Write LOW
	}
	
	//Mode 5: Sixteenth Step, (MS1 = high) (MS2 = high) (MS3 = high)
	else if (mode == 5)
	{
		MS1_PORT |= (1<<MS1_PORT_BIT);	//write HIGH
		MS2_PORT |= (1<<MS2_PORT_BIT);	//write HIGH
		MS3_PORT |= (1<<MS3_PORT_BIT);	//Write HIGH
	}
	
	else
	{
		p_serial->puts("Invalid step_mode, defaulting to Full Step - 2Phase");
		MS1_PORT &= ~(1<<MS1_PORT_BIT);	//write LOW
		MS2_PORT &= ~(1<<MS2_PORT_BIT);	//Write LOW
		MS3_PORT &= ~(1<<MS3_PORT_BIT);	//Write LOW
	}
	
}

//-------------------------------------------------------------------------------------
/** This method sets up PWM related registers
 *  
 */
void stepper::pwm_setup()
{
	if (pwm_set == 0)	//set up PWM if it's not setup already.
	{
		//Enable output on Port H, Pin 3 and 4
		DDRH |= (1<<DDH3) | (1<<DDH4);
		
		
		//Waveform Generation: Fast PWM, Top set by OCR4A (ie. freq)
		TCCR4A |= (1<<WGM40) | (1<<WGM41);
		TCCR4B |= (1<<WGM42) | (1<<WGM43);
		
		//Compare Output Mode: Clear on compare match - non-inverting
		TCCR4A |= (1<<COM4A1) | (1<<COM4B1);
		
		//Pre-scaler: 256
		TCCR4B |= (1<<CS42);
		
		TIMSK4 |= (1<<TOIE4);
		
		//Setup the Output pulse width. We don't need to change this.
		//For Easy Stepper driver, we just need to generate pulse. Change freq by OCR4A
		OCR4B = 1;
		
		//Stop motor. This will control frequency by writing to OCR4A
		stop();
		
		//change pwm set up flag to 1 so you don't setup again.
		pwm_set = 1;		
		
		*p_serial <<endl <<"PWM setup complete!!";
	}
	
	else
	{
		*p_serial <<endl <<"PWM is already set!!";
	}
}

//Constructor
//-------------------------------------------------------------------------------------
/** This is the constructor of stepper class. Here the pwm_setup() method, step_mode() methods are called
 *	to setup the stepper motor. 
 *  @param 
 *  @return
 */
stepper::stepper(time_stamp* p_stamp, base_text_serial* p_ser, task_timer* p_time, unsigned int number_of_steps)
{
	p_time_stamp = p_stamp;				// copy time_stamp pointer
	p_serial = p_ser;					// copy serial pointer
	p_timer = p_time;					// copy task timer pointer
	pwm_set = 0;						// Initialize variable: pwm is not set yet
	steps_per_rev = number_of_steps;	// copy variable number of steps
	step_delay = 70;					// set stepping delay to 70us
	
	pwm_setup();						// Setup PWM settings
	
	//setup the Direction Pin to output
	DIR_PIN_DDR |= (1<<DIR_BIT);
	
	//set step mode to Full step by default
	//You can change it by calling step_mode method.
	step_mode(1);
	
	
	
	//timer_overflow = 0;
	
}

//-------------------------------------------------------------------------------------
/** 
 *  @param 
 *  @return
 */
void stepper::step(bool direction, unsigned long steps_to_go, unsigned int at_what_speed)
{
	
	steps = steps_to_go;
	
	if (direction)
	{
		forward();
		timer_overflow = 0;
		set_speed(at_what_speed);
	}
	
	else
	{
		reverse();
		timer_overflow = 0;
		set_speed(at_what_speed);
	}
	
	
}


//-------------------------------------------------------------------------------------
/** This method turns off PWM by setting it to 0
 *  @param no input parameter
 *  @return no output parameter
 */
void stepper::pwm_off()
{
	write_16bit(0);
}

/**set the speed of pwm
 *	@param uint16_t	16 bit speed setting.
 *	@return no output parameter
 */
void stepper::set_speed(uint16_t sp)
{
	write_16bit(sp);
	//*p_serial << endl << "Speed: " << dec << sp<<endl;
	
}


//-------------------------------------------------------------------------------------
/** This method moves the stepper motor in forward direction
 *  @param no input parameter
 *  @return no output parameter
 */
void stepper::forward()
{
	DIR_PORT |= (1<<DIR_PORT_BIT);	//write 1 to DIR_PORT connected to DIR pin on easydriver
}


//-------------------------------------------------------------------------------------
/** This method moves stepper motor in reverse direction
 *  @param no input parameter
 *  @return no output parameter
 */
void stepper::reverse()
{
	DIR_PORT &= ~(1<<DIR_PORT_BIT);	//write 0 to DIR_PORT connected to DIR pin on easydriver
}


//-------------------------------------------------------------------------------------
/** This method stops motor
 *  @param no input parameter
 *  @return no output parameter
 */
void stepper::stop()
{
	pwm_off();
}


//-------------------------------------------------------------------------------------
/** This method initializes motor to left
 *  @param no input parameter
 *  @return no output parameter
 */
 
/*	set_speed 
 *	forward()
 *	while(_left sensor is off, step (10)
 *	if (_left sensor is on, step(1), check, step 1 at a time until left-sensor off.

*/
void stepper::initialize()
{
	
	//*p_serial <<endl << bin << LSTOP_SENSOR_PIN <<endl;
	
	//if (PINC & (1<<PC5))
	if (LSTOP_SENSOR_PIN & (1<<LSTOP_SENSOR_PIN_BIT))
	{
		unsigned char a_byte = (PINC & (1<<PC5));
		*p_serial << endl << dec << a_byte <<endl;
	}
	
	//while (PINC & (1<<PC5))
	while (LSTOP_SENSOR_PIN & (1<<LSTOP_SENSOR_PIN_BIT))
	{
		//*p_serial << endl << "going in while init loop" << endl;
		//step (direction (1 for clockwise), steps to go, speed)
		step(1,1,1000);
		// if(~(LSTOP_SENSOR_PIN &(1<<LSTOP_SENSOR_BIT)))
		// {
			// break;
		// }
		
	}	
}



