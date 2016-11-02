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

#ifndef _INTERVELOMETER_H_
#define _INTERVELOMETER_H_                     	///< Prevents multiple inclusion of file

extern volatile unsigned int shutter_compare; 	// variable for keeping track of shutter timer. 
extern volatile unsigned int shutter_speed;		//shutter_frequency
extern volatile bool inTakePicMode;

class intervelometer
{
	
     protected:
	
		time_stamp* p_time_stamp;		//Variable to store passed time_stamp pointer
        base_text_serial* p_serial;		//Variable to store passed serial port pointer
		task_timer* p_timer;			//Variable to store passed task_timer
		
		void pwm_setup();				//Protected method for setting up pwm timer
		
	private:
		uint16_t read_16bit();			//Private method for read from 16bit register
		void write_16bit(uint16_t);		//Private method to write to 16bit register

		
     public:
		intervelometer(time_stamp* , base_text_serial* , task_timer*);
		void SetTimelapse(unsigned int);
		void SetPicDelay(unsigned int);
		void SetMotorDelay(unsigned int);
		void stop_timer();
		void take_pic();
		void delay_loop();

};


#endif

// following line turns on automatic (because I am lazy or smart, there is a fine line) indentation for Kate editor.
// kate: space-indent on; indent-width 5; mixedindent off; indent-mode cstyle;