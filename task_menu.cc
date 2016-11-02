//*************************************************************************************
/** \file task_menu.cc
 *	This task is used to for lcd_menu. It displays the menu and it allows the buttons to change
 *	parameters for timelapse. This is adaptation of menu code by Davide Gironi. It is simply
 * 	turned into the task format.
 *
 *   Author: Vrajesh Patel
 *   -------------------------------STATES DEFINITION----------------------------------
 *   State 0 = Initialize
 *   State 1 = Calculate Period and compare to lower and upper limit. If inside the limit then change v_found to true.
 *
 *
 *
 *  task_victim_detect:
 *   \li  06-07-2015     VP        Initial Version created
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto. 
 */
//*************************************************************************************
#define F_CPU 16000000UL

#include <stdlib.h>				//standard avr library
#include <avr/io.h>             //standard avr io library
#include <string.h>				//standard library
#include <avr/interrupt.h>		//standard library
#include <avr/eeprom.h>			//standard library
#include <avr/pgmspace.h>		//standard library	
#include <util/delay.h>			//standard library

#include "rs232.h"				//custom library for USB serial communication
#include "avr_adc.h"			//custom library for reading from ADC

#include "stl_timer.h"			//custom library for handling timer control.
#include "stl_task.h"			//custom library for handling creating tasks.

#include "lcd.h"				//custom library for printing things to lcd screen
#include "micromenu.h"			//custom library for creating menu system
#include "lcdmenu1.h"			//custom library for creating menu system - part of micromenu
#include "menu.h"				//custom library for creating menu system - part of micro menu
#include "task_menu.h"   		//.h file for this task menu class. <this class>

//def int number of buttons
#define BUTTON_NUM 5

//define which button is which.
#define BUTTON_RIGHT 0
#define BUTTON_UP 1
#define BUTTON_DOWN 2
#define BUTTON_LEFT 3
#define BUTTON_SELECT 4

//define adc read max values for button input
#define BUTTON_ADCRIGHT 50
#define BUTTON_ADCUP 250
#define BUTTON_ADCDOWN 350
#define BUTTON_ADCLEFT 550
#define BUTTON_ADCSELECT 850

//button channel
#define BUTTON_CHANNEL 5

//button press delay, prevent multiple keypress
#define BUTTON_PRESSDELAYMS 300

//button repetition counter
#define BUTTON_PRESSCOUNTMAX10 10
#define BUTTON_PRESSCOUNTMAX100 20
#define BUTTON_PRESSCOUNTMAX 20 //max between above definitions

//-------------------------------------------------------------------------------------
/*
 * get button pressed
 */
int16_t get_button(uint16_t buttonadc) 
{
	int16_t ret = -1;
	uint8_t i = 0;
	uint16_t buttonsarray[BUTTON_NUM] = { BUTTON_ADCRIGHT, BUTTON_ADCUP, BUTTON_ADCDOWN, BUTTON_ADCLEFT, BUTTON_ADCSELECT };
	for(i=0; i<BUTTON_NUM; i++) {
		if(buttonadc < buttonsarray[i]) {
			ret=i;
			break;
		}
	}
	return ret;
}


//-------------------------------------------------------------------------------------
     /** This constructor creates a victim detection task object. The victim detection object needs pointers to
     *  an A/D converter, a serial port , time-stamp and task_timer.  This object takes a binary signal from 
     * PORTC and calulates the frequency uising a falling edge detection algorithm and recordes the time at 
     *  which it happens, the next time a falling edge is detected a second time reading is taken.  The tow 
     * are subtracted and the period of the incoming signal is found.  Then upper and lower threshold values
     * are set and a victim detection variable is set if the signal is within the upper ane lower bounds. 
     *  port pointer is handy for debugging. 
     *  @param t_stamp 	A timestamp which contains the time between runs of this task
     *  @param p_a2d	A pointer to an analog to digital converter object
     *  @param p_ser	A pointer to a serial port for sending messages (default NULL)
     *  @param p_timer	A pointer to task_timer for capturing current time and calculating period.
     */

task_menu::task_menu (time_stamp* t_stamp, base_text_serial* p_ser, task_timer* p_timer, avr_adc* p_adc_t)
	: stl_task (*t_stamp, p_ser)
{
	
	// Save pointers to other objects
	p_serial = p_ser;
	p_time_stamp = t_stamp;
	p_get_time = p_timer;
	p_adc = p_adc_t;
    
    
}


//-------------------------------------------------------------------------------------
/** This is the function which runs when it is called by the task scheduler. It causes
 *  victom detection task to run.
 *  @param state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *	  transition is called for at this time
 */

char task_menu::run (char state)
{
	

	switch (state)
	{
		// In State 0, init state. this state clears and sets 
	     case (0):
        {

			//init interrupt
			sei();

			uint16_t button_adc = 0;

			int8_t button_press = 0;
			int8_t button_pressprev = 0;
			//*p_serial << endl << GetMotorRPM();
			menuitem_initialize();

			return(1);

				break;
		}
		
		// State 1 is for reading button presses and cycle through menu system.
		// Also comparison and change of state is done in here.
		case (1):
		{

			button_adc = p_adc->read_once(BUTTON_CHANNEL,0);

			//get button pressed
			button_press = get_button(button_adc);

			//simple timer count for multipress
			if(button_pressprev == button_press) 
			{
				if(button_presscount <= BUTTON_PRESSCOUNTMAX)
					button_presscount++;
			} 
			else 
			{
				button_presscount = 0;
			}

			//evaluate pressed button
			if(button_press != -1) 
			{
				//precess button
				switch(button_press) 
				{
					case BUTTON_RIGHT:
						lcdmenu1_RIGHT();
						break;
					case BUTTON_UP:
						lcdmenu1_UP();
						break;
					case BUTTON_DOWN:
						lcdmenu1_DOWN();
						break;
					case BUTTON_LEFT:
						lcdmenu1_LEFT();
						break;
					case BUTTON_SELECT:
						lcdmenu1_SELECT();
						break;
				}

				_delay_ms(BUTTON_PRESSDELAYMS); //keypress timeout
			}

			//update previous pressed button
			button_pressprev = button_press;
		
		
			 
			return (STL_NO_TRANSITION);
		
		break;
		}
		// If the state isn't a known state, call Houston; we have a problem
		default:
			STL_DEBUG ("WARNING: Menu System task in state " << state << endl);
			return (0);
	};

	// If we get here, no transition is called for
	return (STL_NO_TRANSITION);
}



// following line turns on automatic (because I am lazy or smart, there is a fine line) indentation for Kate editor.
// kate: space-indent on; indent-width 5; mixedindent off; indent-mode cstyle;