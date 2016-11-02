//*************************************************************************************
/** \file task_menu.h
 *    This file contains a task class for detecting victom (LED blinking at 60Hz)
 *   The are several functions 
 *  Revisions:
 *    \li 06-07-2015 VP		Original file
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto. 
 */
//*************************************************************************************

/// This define prevents this .h file from being included more than once in a .cc file

#ifndef _TASK_MENU_H_
#define _TASK_MENU_H_

//-------------------------------------------------------------------------------------
/** This class contains a task which detects whether sensor has detected 60Hz pulse on
 *	PORTC, PINC3
 */


class task_menu : public stl_task
{
	protected:
		
		//Pointers
		base_text_serial* p_serial;		///< Pointer to a serial port for messages
		time_stamp* p_time_stamp;		///< Pointer to a time_stamp for storing time_stamp variable.     
		task_timer* p_get_time;			///< Pointer to task_timer - cooperative multitasking class
		avr_adc* p_adc;					///< Pointer to analog to digital converter class for reading menu navigation buttons.
		
		
	
	public:
		// The constructor creates a new task object
		task_menu (time_stamp*,  base_text_serial*, task_timer*, avr_adc*);
          char run (char);
		
		uint16_t button_adc;
		int8_t button_press;
		int8_t button_pressprev;
	
};

#endif

// following line turns on automatic (because I am lazy or smart, there is a fine line) indentation for Kate editor.
// kate: space-indent on; indent-width 5; mixedindent off; indent-mode cstyle;
