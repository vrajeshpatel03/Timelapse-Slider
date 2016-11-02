//*************************************************************************************
/** \file task_navigation.cc
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
#include "stepper.h"			//custom library for using stepper motor
#include "micromenu.h"			//custom library for creating menu system
#include "lcdmenu1.h"			//custom library for creating menu system - part of micromenu
#include "menu.h"				//custom library for creating menu system - part of micro menu
#include "intervelometer.h"		//custom library for intervelometer 
#include "task_navigation.h"   		//.h file for this task menu class. <this class>


#define LSTOP_SENSOR_DDR		DDRC		//Data Direction Register for Left stop sensor
#define LSTOP_SENSOR_DDR_BIT	DDC5		//Bit for Data direction register for left stop sensor
#define LSTOP_SENSOR_PIN		PINC		//Pin for reading state of the left stop sensor
#define LSTOP_SENSOR_PIN_BIT	PINC5		//pin register for reading state of the left stop sensor

#define RSTOP_SENSOR_DDR		DDRC		//Data Direction Register for Left stop sensor
#define RSTOP_SENSOR_DDR_BIT	DDC6		//Bit for Data direction register for left stop sensor
#define RSTOP_SENSOR_PIN		PINC		//Pin for reading state of the left stop sensor
#define RSTOP_SENSOR_PIN_BIT	PINC6		//pin register for reading state of the left stop sensor

//-------------------------------------------------------------------------------------
     /** This constructor creates a victim detection task object. The victim detection object needs pointers to
     *  an A/D converter, a serial port , time-stamp and task_timer.  This object takes a binary signal from 
     * PORTC and calulates the frequency uising a falling edge detection algorithm and recordes the time at 
     *  which it happens, the next time a falling edge is detected a second time reading is taken.  The tow 
     * are subtracted and the period of the incoming signal is found.  Then upper and lower threshold values
     * are set and a victim detection variable is set if the signal is within the upper ane lower bounds. 
     *  port pointer is handy for debugging. 
     *  @param t_stamp 	A timestamp which contains the time between runs of this task
     *  @param p_ser	A pointer to a serial port for sending messages (default NULL)
     *  @param p_timer	A pointer to task_timer for capturing current time and calculating period.
	 *  @param p_adc_t	A pointer to adc converter.
     */

task_navigation::task_navigation (time_stamp* t_stamp, base_text_serial* p_ser, task_timer* p_timer, avr_adc* p_adc_t, stepper* p_stepper_t, intervelometer* p_intervelometer_t)
	: stl_task (*t_stamp, p_ser)
{
	
	// Save pointers to other objects
	p_serial = p_ser;			//serial
	p_time_stamp = t_stamp;		//timestamp
	p_get_time = p_timer;		//timer
	p_adc = p_adc_t;			//ADC
    p_stepper = p_stepper_t;	//stepper motor
	p_intervelometer = p_intervelometer_t;	//intervelometer
    
}


//-------------------------------------------------------------------------------------
/** This is the function which runs when it is called by the task scheduler. It causes
 *  navigation task sto run.
 *  @param state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *	  transition is called for at this time
 */

char task_navigation::run (char state)
{
	

	switch (state)
	{
		// State 0: Init state where variables and other things are iniatilzed
		//			ready to go.
	     case (0):
        {
			
			//initilization
			
			p_stepper->stop();
			//variables
			// init_left = false;
			// init_right = false;
			
			numberOfRevs = 0;
			totalSteps = 0;
			totalTravelTime = 0;
			totalNumberOfPics = 0;
			stepsPerPic = 0;
			currentPicNumber = 0;
	
			//Set Right stop sensor to input.
			//DDRC &= ~(1<<DDC6);
			RSTOP_SENSOR_DDR &= ~(1<<RSTOP_SENSOR_DDR_BIT);
			
			//Set Left stop sensor to input.
			//DDRC &= ~(1<<DDC5);
			LSTOP_SENSOR_DDR &= ~(1<<LSTOP_SENSOR_DDR_BIT);
			
			
			//Always return 1
			return(1);

			break;
		}
		
		// State 1: Waiting for User Input
		case (1):
		{
			//*p_serial <<endl << "nav case 1: Waiting..";
			//*p_serial <<endl << "init_left: " << init_left;
			//*p_serial <<endl << "init_right: " << init_right;
			
			p_stepper->stop();
			
			//Get the timerCount so we know how fast to move motor based on RPM.
			
			den = GetMotorRPM() * GetStepsPerRev();
			den = den * 256;
			num = F_CPU * 60;
			timerCount = num/den;
			
			//timerCount = (F_CPU * 60) / (256 * GetMotorRPM() * GetStepsPerRev());
			
			//*p_serial <<endl << "Num: " <<num;
			//*p_serial <<endl << "Den: " <<den;
			//*p_serial <<endl << "timer: " <<timerCount;
			
			
			//init left will be set to true if youre inside the init left submenu
			if (init_left == 1) 
			{
				
				return(2);
			}
			
			if (init_right == 1)
			{
				
				return(3);
			}
			
			if (startTimelapse == 1) 
			{
				
				return(4);
			}
				
			else {
				
				return (STL_NO_TRANSITION);
			}
		
			break;
		}
		
		// State 2: Initialize Left
		case (2):
		{
			//check to see if the left sensor has been hit.
			if (LSTOP_SENSOR_PIN & (1<<LSTOP_SENSOR_PIN_BIT)) 
			{				
				//Stop the motor and set init_left to false so you know it's finished.
				p_stepper->stop();
				init_left = 0;
				
			} 

			if (init_left == 0) 
			{
				return(1);
			}
			
			else if (inMoveMotorMode == false)
			{
				//*p_serial <<endl << "nav case 3: Init Right";
				p_stepper->step(1, 10, timerCount);
			}
			
			else 
			{
				return (STL_NO_TRANSITION);
			}
		
			break;
		}
		
		// State 3: Initialize Right
		case (3):
		{
			//check to see if the left sensor has been hit.
			if (RSTOP_SENSOR_PIN & (1<<RSTOP_SENSOR_PIN_BIT)) 
			{
				//Stop the motor and set init_right to false so you know it's finished.
				p_stepper->stop();
				init_right = 0;
				
			}
			
			
			else if (init_right == 0) 
			{
				//*p_serial <<endl << "nav case 3: Init Right44444";				
				return(1);
			}
			
			
			else if (inMoveMotorMode == false)
			{
				//*p_serial <<endl << "nav case 3: Init Right";
				p_stepper->step(0, 10, timerCount);
			}
			
			
			else {
				//*p_serial <<endl << "nav case 3: Init Right33333";
				//*p_serial <<endl << "In Move Motor: " << inMoveMotorMode;
				//*p_serial <<endl << "StartTimelapse: " << startTimelapse;

				return (STL_NO_TRANSITION);
			}
		
			break;
		}
		
		// State 4: Start Timelapse
		case (4):
		{
			
			//Grab information from lcd to figure out how many pictures you can take and also how many
			//  steps to move in between pictures.
			
			if (stepsPerPic == 0)
			{
				//-----------------------------------------------
				num = GetTrackLength();
				num = num * 1000;
				den = GetPitch() * GetTeeth();
				
				numberOfRevs = num/den;
				//numberOfRevs = (1000 * GetTrackLength()) / (GetPitch() * GetTeeth());
				*p_serial <<endl << "Number of Revs = " <<numberOfRevs;
				
				
				//-----------------------------------------------
				num = GetTrackLength();
				num = num * GetStepsPerRev();
				num = num * 1000;
				den = GetPitch()*GetTeeth();
				//*p_serial <<endl << "Num = " <<num;
				//*p_serial <<endl << "Den = " <<den;
				
				totalSteps = num/den;
				//totalSteps = (1000 * GetTrackLength() * GetStepsPerRev()) / (GetPitch()*GetTeeth());
				*p_serial <<endl << "Total Steps = " <<totalSteps;
				
				//-----------------------------------------------
				num = GetTrackLength();
				num = num * 60 * 1000;
				den = GetPitch();
				den = den * GetTeeth();
				den = den * GetMotorRPM();
				//*p_serial <<endl << "Num = " <<num;
				//*p_serial <<endl << "Den = " <<den;
				
				totalTravelTime = num/den;
				//totalTravelTime = (60*GetTrackLength()*1000) / (GetPitch() * GetTeeth() * GetMotorRPM());
				*p_serial <<endl << "Total Travel Time = " <<totalTravelTime;
				
				//-----------------------------------------------
				//num = 
				totalNumberOfPics = (60*GetTimelapsePeriod() - totalTravelTime) / (GetShutterSpeed() + GetPicDelay() + 2 * GetMotorDelay());
				*p_serial <<endl << "Total Number of Pics = " <<totalNumberOfPics;
				
				//-----------------------------------------------
				stepsPerPic = totalSteps / totalNumberOfPics;
				*p_serial <<endl << "Steps Per Pic = " <<stepsPerPic;
			
			}
			
			//If timelapse taking mode and also number of pics are less than total possible number of pics.
			if ((startTimelapse == 1) && (currentPicNumber <= totalNumberOfPics))
			{
				lastPicNumber = 0;
				return(5);
			}
			
			if (startTimelapse == 0) 
			{
				
				//go back to waiting status.
				return(1);
			}
		
			else 
			{
				
				return (STL_NO_TRANSITION);
			}
		
			break;
		}
		
		//State 5: Take Pic
		case (5):
		{  
			if (currentPicNumber >= totalNumberOfPics)
			{
				//go back to waiting status.
				*p_serial <<endl <<"going back to waiting status";
				startTimelapse = 0;
				return(1);
			}
			
			else if (startTimelapse == 0) 
			{
				
				//go back to waiting status.
				return(1);
			}

			else 
			{
				*p_serial <<endl <<"Taking Pic and going to PicDelayMode";
				inTakePicMode = true;
				p_intervelometer->SetTimelapse(GetShutterSpeed());
				p_intervelometer->take_pic();
				currentPicNumber++;
				return (6);
			}
		
			break;
		}
		
		//State 6: Pic Delay Mode
		case (6):
		{
			if (inTakePicMode == false) 
			{
				*p_serial <<endl <<"Starting Pic Delay and going to MotorDelayMode";
				inPicDelayMode = true;
				p_intervelometer->SetPicDelay(GetPicDelay());
				p_intervelometer->delay_loop();
				return(7);
			}
			
			else if (startTimelapse == 0) 
			{
				
				//go back to waiting status.
				return(1);
			}
			
			else 
			{
				//*p_serial <<endl <<"insidePicDelayMode: " <<inTakePicMode;
				return (STL_NO_TRANSITION);
			}
		
			break;
		}
		
		//State 7: Motor Move Delay
		case (7):
		{
			if ((inMoveMotorMode == false) && (inPicDelayMode == false) && (motorMoveComplete == true))
			{
				*p_serial <<endl <<"Starting Motor Delay and going to TakePicMode";
				p_intervelometer->SetMotorDelay(GetMotorDelay());
				p_intervelometer->delay_loop();
				motorMoveComplete = false;
				return(5);
			}
			
			else if ((inMoveMotorMode == false) && (inPicDelayMode == false) && (motorMoveComplete == false))
			{
				*p_serial <<endl <<"Starting Motor Delay and going to MoveMotorMode";
				inMotorDelayMode = true;
				p_intervelometer->SetMotorDelay(GetMotorDelay());
				p_intervelometer->delay_loop();
				return(8);
			}
			
			else if (startTimelapse == 0) 
			{
				
				//go back to waiting status.
				return(1);
			}
			
			else 
			{
				return (STL_NO_TRANSITION);
			}
		
			break;
		}
		
		//State 8: Motor Move
		case (8):
		{
			if (inMotorDelayMode == false)	
			{	
				*p_serial <<endl <<"Moving Motor and going to MotorDelayMode";	
				inMoveMotorMode = true;
				p_stepper->step(1, stepsPerPic, timerCount);
				return (7);
			}
			
			else if (startTimelapse == 0) 
			{
				
				//go back to waiting status.
				return(1);
			}

			else 
			{
				
				return (STL_NO_TRANSITION);
			}
		
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