//**************************************************************************************
/** \file stl_timer.cc
 *    This file contains a class which runs a task timer whose resolution is one
 *    microsecond. The timer is used to help schedule the execution of tasks' run()
 *    functions. The functions can be run from a main loop by checking periodically 
 *    if a task time has expired, or they can be called directly from the timer 
 *    interrupt service routine in this file, or they can be called from some other 
 *    hardware interrupt service routine; in the last case this file isn't involved. 
 *
 *  Revisions:
 *    \li 08-07-2007 JRR Created this file as daytimer.* with 1 second interrupts
 *    \li 08-08-2007 JRR Added event triggers
 *    \li 12-23-2007 JRR Made more general by allowing faster interrupt rates
 *    \li 01-05-2008 JRR Converted from time-of-day version to microsecond version
 *    \li 03-27-2008 JRR Added operators + and - for time stamps
 *    \li 03-31-2008 JRR Merged in stl_us_timer (int, long) and set_time (int, long)
 *    \li 05-15-2008 JRR Changed to use Timer 3 so Timer 1 can run motor PWM's
 *    \li 05-31-2008 JRR Changed time calculations to use CPU_FREQ_MHz from Makefile
 *    \li 01-04-2009 JRR Now uses CPU_FREQ_Hz (rather than MHz) for better precision
 *
 *  License:
 *    This file copyright 2007 by JR Ridgely. It is released under the Lesser GNU
 *    public license, version 2.
 */
//**************************************************************************************

#include <stdlib.h>							// Used for itoa()
#include <string.h>							// Header for character string functions
#include <avr/interrupt.h>					// For using interrupt service routines

#include "base_text_serial.h"				// Base for text-type serial port objects
#include "stl_timer.h"						// Header for this file


//--------------------------------------------------------------------------------------
// This variables is only used to allow the interrupt service routine to update the
// measured time whenever a timer interrupt occurs, and allow a timer object to read
// the time. The user should not have any reason to read or write it.

/** This variable holds the number of times the hardware timer has overflowed. This
 *  number is equivalent to the upper 16 bits of a 32-bit timer, and is so used. */

unsigned int ust_overflows = 0;


//--------------------------------------------------------------------------------------
/** This constructor creates a time stamp object, with the time set to zero. 
 */

time_stamp::time_stamp (void)
{
	set_time (0L);
}


//--------------------------------------------------------------------------------------
/** This constructor creates a time stamp object and fills the time stamp's variables
 *  with the given values.
 *  @param a_time A 32-bit time number with which the time stamp is filled
 */

time_stamp::time_stamp (long a_time)
{
	set_time (a_time);
}


//--------------------------------------------------------------------------------------
/** This constructor creates a time stamp object and fills the time stamp's variables
 *  with the number of seconds and microseconds given. 
 *  @param sec A 16-bit number of seconds to preload into the time stamp
 *  @param microsec A 32-bit number of microseconds to preload into the time stamp
 */

time_stamp::time_stamp (int sec, long microsec)
{
	set_time (sec, microsec);
}


//--------------------------------------------------------------------------------------
/** This method fills the time stamp with the given value.
 *  @param a_time A 32-bit time number with which the time stamp is filled
 */

void time_stamp::set_time (long a_time)
{
	data.whole = a_time;
}


//--------------------------------------------------------------------------------------
/** This method fills the time stamp with the given numbers of seconds and microseconds.
 *  @param sec A 16-bit number of seconds to preload into the time stamp
 *  @param microsec A 32-bit number of microseconds to preload into the time stamp
 */

void time_stamp::set_time (int sec, long microsec)
{
	data.whole = (microsec * (CPU_FREQ_Hz / 1000000UL)) / 8UL;
	data.whole += (unsigned int)sec * (CPU_FREQ_Hz / 8UL);
}


//--------------------------------------------------------------------------------------
/** This method allows one to get the time reading from this time stamp as a long 
 *  integer containing the number of time ticks. 
 *  @return The time stamp's raw data
 */

long time_stamp::get_raw_time (void)
{
	return (data.whole);
}


//--------------------------------------------------------------------------------------
/** This method returns the number of seconds in the time stamp.
 *  @return The number of whole seconds in the time stamp
 */

int time_stamp::get_seconds (void)
{
	return ((int)(data.whole / (CPU_FREQ_Hz / 8)));
}


//--------------------------------------------------------------------------------------
/** This method returns the number of microseconds in the time stamp, after the seconds
 *  are subtracted out. 
 *  @return The number of microseconds, that is, the fractional part of the time stamp
 */

long time_stamp::get_microsec (void)
{
	return ((data.whole % (CPU_FREQ_Hz / 8UL)) / ((CPU_FREQ_Hz / 1000000UL) / 8UL));
}


//--------------------------------------------------------------------------------------
/** This overloaded addition operator adds another time stamp's time to this one.  It 
 *  can be used to find the time in the future at which some event is to be caused to 
 *  happen, such as the next time a task is supposed to run. 
 *  @param addend The other time stamp which is to be added to this one
 *  @return The newly created time stamp
 */

time_stamp time_stamp::operator + (const time_stamp& addend)
{
	time_stamp ret_stamp;
	ret_stamp.data.whole = this->data.whole + addend.data.whole;

	return ret_stamp;
}


//--------------------------------------------------------------------------------------
/** This overloaded subtraction operator finds the duration between this time stamp's 
 *  recorded time and a previous one. 
 *  @param previous An earlier time stamp to be compared to the current one 
 *  @return The newly created time stamp
 */

time_stamp time_stamp::operator - (const time_stamp& previous)
{
	time_stamp ret_stamp;
	ret_stamp.data.whole = this->data.whole - previous.data.whole;

	return ret_stamp;
}


//--------------------------------------------------------------------------------------
/** This overloaded addition operator adds another time stamp's time to this one.  It 
 *  can be used to find the time in the future at which some event is to be caused to 
 *  happen, such as the next time a task is supposed to run. 
 *  @param addend The other time stamp which is to be added to this one
 */

void time_stamp::operator += (const time_stamp& addend)
{
	data.whole += addend.data.whole;
}


//--------------------------------------------------------------------------------------
/** This overloaded subtraction operator finds the duration between this time stamp's 
 *  recorded time and a previous one. Note that the data in this timestamp is 
 *  replaced with that duration. 
 *  @param previous An earlier time stamp to be compared to the current one 
 */

void time_stamp::operator -= (const time_stamp& previous)
{
	data.whole -= previous.data.whole;
}


//--------------------------------------------------------------------------------------
/** This overloaded equality test operator checks if all the time measurements in some
 *  other time stamp are equal to those in this one. 
 *  @param other A time stamp to be compared to this one 
 *  @return True if the time stamps contain equal data, false if they don't
 */

bool time_stamp::operator == (const time_stamp& other)
{
	if (other.data.whole == data.whole)
		return (true);
	else
		return (false);
}


//--------------------------------------------------------------------------------------
/** This overloaded inequality operator checks if this time stamp is greater than or
 *  equal to another. If the user wants to check for less-than, negating the result of
 *  this method is a lot easier (and more efficient) than writing another one. The
 *  method used to check greater-than-ness needs to work across timer overflows, so
 *  the following technique is used: subtract the other time stamp from this one as
 *  unsigned 32-bit numbers, then check if the result is positive (in which case this 
 *  time is greater) or not. 
 *  @param other A time stamp to be compared to this one 
 *  @return True if this time stamp is greater than or equal to the other one
 */

bool time_stamp::operator >= (const time_stamp& other)
{
	long difference;						// Vive la difference

	difference = data.whole - other.data.whole;

	if ((signed long)difference > 0L)
		return (true);
	else
		return (false);
}


//--------------------------------------------------------------------------------------
/** This constructor creates a daytime task timer object.  It sets up the hardware timer
 *  to count at ~1 MHz and interrupt on overflow. Note that this method does not enable
 *  interrupts globally, so the user must call sei() at some point to enable the timer
 *  interrupts to run and time to actually be measured. 
 */

task_timer::task_timer (void)
{
	#ifdef TCNT3							// If Timer 3 exists, we'll use it
		TCCR3A = 0x00;						// Set to normal 16-bit counting mode
		TCCR3B = (1 << CS31);				// Set prescaler to main clock / 8
		TIMSK3 |= (1 << TOIE3);				// Set Timer 3 overflow interrupt enable
	#else
		TCCR1A = 0x00;						// Set to normal 16-bit counting mode
		TCCR1B = (1 << CS11);				// Set prescaler to main clock / 8
		#ifdef TIMSK1						// If there's a TIMSK1, set it
			TIMSK1 |= (1 << TOIE1);			// Enable Timer 1 overflow interrupt
		#else								// If no TIMSK1, there's a TIMSK
			TIMSK |= (1 << TOIE1);			// Enable Timer 1 overflow interrupt
		#endif
	#endif
}


//--------------------------------------------------------------------------------------
/** This method grabs the current time stamp from the hardware and overflow counters. 
 *  In order to prevent the data changing during the time when it's being read (which 
 *  would cause invalid data to be saved), interrupts are disabled while the time data 
 *  is being copied. 
 *  @param the_stamp Reference to a time stamp variable which will hold the time
 */

void task_timer::save_time_stamp (time_stamp& the_stamp)
{
	cli ();									// Prevent interruption
	the_stamp.data.half[0] = TMR_TCNT_REG;	// Get hardware count
	the_stamp.data.half[1] = ust_overflows;	// Get overflow counter data
	sei ();									// Re-enable interrupts
}


//--------------------------------------------------------------------------------------
/** This method saves the current time in the internal time stamp belonging to this 
 *  object, then returns a reference to the time stamp so that the caller can use it as
 *  a measurement of what the time is now. 
 */

time_stamp& task_timer::get_time_now (void)
{
	cli ();									// Prevent interruption
	now_time.data.half[0] = TMR_TCNT_REG;	// Get hardware count
	now_time.data.half[1] = ust_overflows;	// Get overflow counter data
	sei ();									// Re-enable interrupts

	return (now_time);						// Return a reference to the current time
}


//--------------------------------------------------------------------------------------
/** This method sets the timer to a given value. It's not likely that this method will
 *  be used, but it is provided for compatibility with other task timer implementations
 *  that measure times of day (in hours, minutes, and seconds) and do need to be set by
 *  user programs. 
 *  @param t_stamp A reference to a time stamp containing the time to be set
 */

bool task_timer::set_time (time_stamp& t_stamp)
{
	cli ();									// Prevent interruption
	TMR_TCNT_REG = t_stamp.data.half[0];
	ust_overflows = t_stamp.data.half[1];
	sei ();									// Re-enable interrupts
}


//--------------------------------------------------------------------------------------
/** This overloaded operator allows a time stamp to be printed on a serial device such
 *  as a regular serial port or radio module in text mode. This allows lines to be set
 *  up in the style of 'cout.' The timestamp is always printed as a decimal number. 
 *  @param serial A reference to the serial-type object to which to print
 *  @param stamp A reference to the time stamp to be displayed
 */

base_text_serial& operator<< (base_text_serial& serial, time_stamp& stamp)
{
	// Get the time in seconds and microseconds
	unsigned int seconds = stamp.get_seconds ();
	unsigned long microseconds = stamp.get_microsec ();

	serial << seconds;
	serial.putchar ('.');

	// For the digits in the fractional part, write 'em in backwards order. We can't
	// use itoa here because we need leading zeros
	for (unsigned long divisor = 100000; divisor > 0; )
	{
		serial.putchar (microseconds / divisor + '0');
		microseconds %= divisor;
		divisor /= 10;
	}

	return (serial);
}


//--------------------------------------------------------------------------------------
/** This is the interrupt service routine which is called whenever there is a compare
 *  match on the 16-bit timer's counter. Nearly all AVR processors have a 16-bit timer
 *  called Timer 1, and this is the one we use here. 
 */

ISR (TMR_intr_vect)
{
	ust_overflows++;
}

/** This function tells you if a certain amount of time has passed.  
 *  If the difference between the time stamps is greater than the second and microseconds you feed
 *  it, it will return true
 */
// bool time_stamp::time_diff (time_stamp init, time_stamp curr,int secs, long micros)
// {
// 	long init_time	= init.get_raw_time();
// 	long curr_time	= curr.get_raw_time();
// 	
// 	init.set_time(curr_time-init_time);	// Set init time stamp to the difference in times
// 	
// 	curr.set_time(secs, micros);	// Set the curr_time stamp to the difference we are looking for
// 	
// 	// If the difference in times is greater than the desired difference, we want to return true
// 	if ((init.get_raw_time()-curr.get_raw_time())>0)
// 	{
// 		return true;
// 	}
// 	return false;
// }
