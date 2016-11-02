//======================================================================================
/** \file timescape.cc
 *      This file contains main() function. Before the main() function, constants,
 *	and other variables are sets. Then the objects are created from various drivers and 
 *	finally the timing of each task is then assigned. After that the infinite mechatronics
 *	loop is started where all tasks are run forever until the Terminator comes back and
 * 	well you know what happens.
 *       
 *
 *  Revisions:
 *	\li	01-10-13  	VP	Initial Version Created	
 *	\li	01-17-13	VP	Sparkfun LCD working...
 *	\li	01-18-13	VP	Adding stepper motor controller
 *	\li	02-09-13	VP	Stepper motor controller working!!
 *
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================


// System headers included with < >
#include <stdlib.h>				// Standard C library
#include <avr/io.h>				// Standar AVR IO library
#include <avr/interrupt.h>		// Interrupt handling functions
#include <avr/eeprom.h>			// Standard EEPROM library

// User written headers included with " "							
#include "rs232.h"				// Include header for serial port class
#include "stl_timer.h"          // Microsecond-resolution timer
#include "stl_task.h"          	// Base class for all task classes
//#include "avr_adc.h"			// Include header for the A/D class
#include "lcd.h"				// Include for sparkfun lcd control with HD44780 controller
#include "stepper.h"			// Include for stepper motor driver 
#include "intervelometer.h"		// Intervelometer code

#include "avr_adc.h"

#include "micromenu.h"
#include "lcdmenu1.h"
#include "menu.h"
#include "task_menu.h"
#include "task_navigation.h"


//Initialize Global Variables
volatile unsigned long timer_overflow; 		//Global variable for keeping track of each timer4 in stepper.h
volatile unsigned long steps;				//Variable that tells you how many steps to move

volatile unsigned int shutter_compare = 0; 	// variable for keeping track of shutter timer. 
volatile unsigned int shutter_speed = 0;	// variable that tells you what shutter speed is
volatile bool inTakePicMode = false;

volatile int16_t button_presscount = 0;		//Varible to for menu system that keeps track number of button presses.


volatile unsigned char init_left = 0;
volatile unsigned char init_right = 0;
volatile unsigned char startTimelapse = 0;
volatile bool inPicDelayMode = false;
volatile bool inMotorDelayMode = false;
volatile bool inMoveMotorMode = false;
volatile bool motorMoveComplete = false;


//--------------------------------------------------------------------------------------
//-------------------Timer Interrupt Subroutine (BEGIN)---------------------------------
 #define PWM_FREQ     	OCR4A


//Interrupt subroutine for checking how many steps has been taken and 
// stop the motor once number of steps has been completed
ISR(TIMER4_OVF_vect)
{
	//update_timer();
	timer_overflow = timer_overflow + 1;
	
	// if (steps > 0)
	// {
		// inMoveMotorMode = true;
	// }
	
	if(steps == timer_overflow)
	{
		PWM_FREQ = 0;
		motorMoveComplete = true;
		steps = 0;
		inMoveMotorMode = false;
	}
}

//Interupt routine for intervelometer.
ISR(TIMER5_COMPC_vect)
{
	shutter_compare = shutter_compare + 1;	//add 1 seconds to it.
	
	//Taking picture mode
	if ((shutter_compare == shutter_speed) && (inTakePicMode == true))
	{
		//toggle pin LOW
		PORTL &= ~(1<<PORTL5);
		
		//PORTL ^= (1<<PORTL5);
		
		//turn off timer by setting pre-scalar 0
		TCCR5B &= ~(1<<CS50);
		TCCR5B &= ~(1<<CS51);
		TCCR5B &= ~(1<<CS52);
		
		//reset shutter compare variable
		shutter_compare = 0;
		inTakePicMode = false;
		
	}
	
	//PicDelay Loop
	else if ((shutter_compare == GetPicDelay()) && (inPicDelayMode == true))
	{
		//turn off timer by setting pre-scalar 0
		TCCR5B &= ~(1<<CS50);
		TCCR5B &= ~(1<<CS51);
		TCCR5B &= ~(1<<CS52);
		
		//reset shutter compare variable
		shutter_compare = 0;
		inPicDelayMode = false;
		
	} 
	
	//Motor Delay Loop
	else if ((shutter_compare == GetMotorDelay()) && (inMotorDelayMode == true))
	{	
		//turn off timer by setting pre-scalar 0
		TCCR5B &= ~(1<<CS50);
		TCCR5B &= ~(1<<CS51);
		TCCR5B &= ~(1<<CS52);
		
		//reset shutter compare variable
		shutter_compare = 0;
		inMotorDelayMode = false;
		
	}
	
	
}


//-------------------Timer Interrupt Subroutine (END)-----------------------------------

//--------------------------------------------------------------------------------------
/** This is the main program that runs our test setup.  It is adaptable to run
 *	various tasks
 *	@return returns 0 if the program ever gets there which means something is wrong.
 */
int main ()
{
	char input_char;

//---------------------------------Initialize Objects-------------------------------------
	//Start serial port
	rs232 the_serial_port (9600, 0);
	the_serial_port << clrscr <<"Timescape Control Module"<< endl
				<< "  CPU Freq: " << CPU_FREQ_Hz/1000000 << " MHz" << endl;
		
	//Create ADC Object
	avr_adc my_adc(&the_serial_port,128,1,5.00);
	
	// Create a microsecond-resolution timer
	task_timer the_timer;
	
	time_stamp interval;
	
	//Initialize LCD Screen and Clear it
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();						//Clear screen
	//lcd_puts("Init Complete\n");		//Write first line
	
	//stepper motor object
	stepper motor (&interval, &the_serial_port, &the_timer, 200);
	
	//intervelometer object
	intervelometer shutter(&interval , &the_serial_port , &the_timer);
		
	
//---------------------------------TASK MENU-------------------------------------
//run task at every 0.005 seconds
	time_stamp interval_time (0,5000);
	 //the_serial_port <<"Menu Task Interval: " << interval_time << " sec or " 
					 //<< interval_time.get_raw_time () << " counts" << endl;
	
	task_menu	menu_system(&interval_time, &the_serial_port, &the_timer, &my_adc);
	
//---------------------------------TASK NAVIGATION-------------------------------
//run task at every 0.0005 seconds
	interval_time.set_time (0, 500);
	//the_serial_port <<"Menu Task Interval: " << interval_time << " sec or " 
					 //<< interval_time.get_raw_time () << " counts" << endl;
	
	task_navigation	timelapse_navigation(&interval_time, &the_serial_port, &the_timer, &my_adc, &motor, &shutter);
	
	sei();	//enable global interrupt
	
	
	while (true)
	{
		//Start the task_menu task.
		menu_system.schedule(the_timer.get_time_now());
		
		//Start the navigation task.
		timelapse_navigation.schedule(the_timer.get_time_now());
	

	}

	return (0);
}



// following line turns on automatic (because I am lazy, or smart, there is a fine line) indentation for Kate editor.
// kate: space-indent on; indent-width 5; mixedindent off; indent-mode cstyle;