//*************************************************************************************
/** \file task_navigation.h
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

#ifndef _TASK_NAVIGATION_H_
#define _TASK_NAVIGATION_H_

//-------------------------------------------------------------------------------------
/** This class contains a task which detects whether sensor has detected 60Hz pulse on
 *	PORTC, PINC3
 */

extern volatile bool inPicDelayMode;
extern volatile bool inMotorDelayMode;
extern volatile bool inMoveMotorMode;
extern volatile bool motorMoveComplete;

class task_navigation : public stl_task
{
	protected:
		
		//Pointers
		base_text_serial* p_serial;			///< Pointer to a serial port for messages
		time_stamp* p_time_stamp;			///< Pointer to a time_stamp for storing time_stamp variable.     
		task_timer* p_get_time;				///< Pointer to task_timer - cooperative multitasking class
		avr_adc* p_adc;						///< Pointer to analog to digital converter class for reading menu navigation buttons.
		stepper* p_stepper;					///< Pointer to stepper motor class.
		intervelometer* p_intervelometer;	///< Pointer to a intervelometer class.
		
		
	
	public:
		// The constructor creates a new task object
		task_navigation (time_stamp*,  base_text_serial*, task_timer*, avr_adc*, stepper*, intervelometer*);
          char run (char);
		
		unsigned long num;
		unsigned long den;

		unsigned int numberOfRevs;
		unsigned int totalSteps;
		unsigned int totalTravelTime;
		unsigned int totalNumberOfPics;
		unsigned int stepsPerPic;
		unsigned int currentPicNumber;
		unsigned int lastPicNumber;
		unsigned int motorSteps;
		unsigned int timerCount;

};
#endif

// following line turns on automatic (because I am lazy or smart, there is a fine line) indentation for Kate editor.
// kate: space-indent on; indent-width 5; mixedindent off; indent-mode cstyle;
