//*************************************************************************************
/** \file stepper.h
 *	
 *  Revisions:
 *	\li	01/19/13	VP	
 *
 *  License:
 *	This file released under the Lesser GNU Public License, version 2. This program
 *	is intended for educational use only, but its use is not limited thereto. 
 */
//*************************************************************************************

#ifndef _STEPPER_H_
#define _STEPPER_H_                     ///< Prevents multiple inclusion of file


extern volatile unsigned long timer_overflow;		//global variable declared in main()
extern volatile unsigned long steps;

class stepper
{
	
     protected:
		time_stamp* p_time_stamp;		//Variable to store passed time_stamp pointer
        base_text_serial* p_serial;		//Variable to store passed serial port pointer
		task_timer* p_timer;			//Variable to store passed task_timer
        //bool brake;					//Variable to store brake
		bool pwm_set;					//Variable to store pwm_set
		unsigned int steps_per_rev;		//Variable to store steps_per_rev
		unsigned int step_delay;		//Variable to store step_delay
		void pwm_setup();				//Protected method for setting up pwm timer
		
		
	private:
		
		uint16_t read_16bit();			//Private method for read from 16bit register
		void write_16bit(uint16_t);		//Private method to write to 16bit register
		
     public:
        stepper(time_stamp*, base_text_serial*, task_timer*, unsigned int);	//Constructor
        void step_mode(unsigned char);					//setting up stepping mode (i.e.. full, half, quarter, eighth stepping mode)
        void pwm_off();									//Method for turning off PWM
		void step(bool, unsigned long, unsigned int);	//Method for incrementing certain number of steps
        void set_speed(uint16_t);						//Method for setting speed of the motor (i.e.. Frequency)
		void forward();									//Method for setting forward direction
		void reverse();									//Method for setting reverse direction
		void stop();									//Method for stopping motor
		void initialize();								//Method for Initializing carriage to left

};


#endif

// following line turns on automatic (because I am lazy or smart, there is a fine line) indentation for Kate editor.
// kate: space-indent on; indent-width 5; mixedindent off; indent-mode cstyle;