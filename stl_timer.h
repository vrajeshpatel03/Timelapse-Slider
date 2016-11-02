//*************************************************************************************
/** \file stl_timer.h
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
//*************************************************************************************

/// These defines prevent this file from being included more than once in a *.cc file
#ifndef _STL_TIMER_H_
#define _STL_TIMER_H_


// Check that the user has set the CPU frequency in the Makefile; if not, complain
#ifndef CPU_FREQ_Hz
	#error The macro CPU_FREQ_Hz must be set in the Makefile.
#endif

// If Timer 3 exists (as on the ATmega128), we use it for the microsecond timer rather
// than Timer 1, as Timer 1 is used for the motor PWM's on the ME405 board
#ifdef TCNT3
	#define TMR_TCNT_REG	TCNT3			///< Register that holds the time count
	#define TMR_intr_vect   TIMER3_OVF_vect	///< The timer overflow interrupt vector 
#else
	#define TMR_TCNT_REG	TCNT1			///< Register that holds the time count
	#define TMR_intr_vect   TIMER1_OVF_vect	///< The timer overflow interrupt vector 
#endif // __AVR_ATmega128__


//--------------------------------------------------------------------------------------
/** This union holds a 32-bit time count. The count can be accessed as a single 32-bit
 *  number, as two 16-bit integers placed together, or as an array of 8-bit characters. 
 *  This is helpful because the time measurement we use consists of the number in a 
 *  16-bit hardware counter and a 16-bit overflow count which should be put together 
 *  into one time number. The characters are handy for looking at individual bits. 
 */

typedef union
{
	long whole;								///< All the data as one 32-bit number
	int half[2];							///< The data as an array of 16-bit ints
	char quarters[4];						///< The data as an array of 8-bit chars
} time_data_32;


//--------------------------------------------------------------------------------------
/** This class holds a time stamp which is used to measure the passage of real time in
 *  the world around an AVR processor. This version of the time stamp implements a 
 *  32-bit time counter that runs at one megahertz. There's a 16-bit number which is
 *  copied directly from a 16-bit hardware counter and another 16-bit number which is 
 *  incremented every time the hardware counter overflows; the combination of the two 
 *  is a 32-bit time measurement. 
 */

class time_stamp
{
	protected:
		/// This union holds the time stamp's data and allows it to be accessed as two
		/// 16-bit halves or one 32-bit whole as required by the application
		time_data_32 data;

	public:
		/// This constructor creates an empty time stamp
		time_stamp (void);

		/// This constructor creates a time stamp and initializes all its data
		time_stamp (long);

		// This constructor creates a time stamp with the given seconds and microsec.
		time_stamp (int, long);

		/// This method fills the timestamp with the given value
		void set_time (long);

		/// This method fills the timestamp with the given seconds and microseconds
		void set_time (int, long);

		/// This method reads out all the timestamp's data as one 32-bit number
		long get_raw_time (void);

		/// This method returns the number of seconds in the time stamp
		int get_seconds (void);

		/// This method returns the number of microseconds in the time stamp
		long get_microsec (void);

		/// This overloaded addition operator adds two time stamps together
		time_stamp operator + (const time_stamp&);

		/// This overloaded subtraction operator finds the time between two time stamps
		time_stamp operator - (const time_stamp&);

		/// This overloaded addition operator adds two time stamps together
		void operator += (const time_stamp&);

		/// This overloaded subtraction operator finds the time between two time stamps
		void operator -= (const time_stamp&);

		/// This overloaded equality operator tests if all time fields are the same
		bool operator == (const time_stamp&);

		/// This overloaded operator tests if a time stamp is greater (later) than this
		bool operator >= (const time_stamp&);
		
		/// RSN - added this function which computes if a certain number of time has passed
		//bool time_diff (time_stamp, time_stamp,int, long);

		// This declaration gives permission for objects of class task_timer to access
		// the private and/or protected data belonging to objects of this class
		friend class task_timer;
};


//--------------------------------------------------------------------------------------
/** This class implements a timer to synchronize the operation of tasks on an AVR. The
 *  timer is implemented as a combination of a 16-bit hardware timer (Timer 1 is the 
 *  usual choice) and a 16-bit overflow counter. The two timers' data is combined to
 *  produce a 32-bit time count which is used to decide when tasks run. WARNING: This
 *  timer does not keep track of the time of day, and it overflows after a little more
 *  than an hour of use. Another timer (see stl_timer.h/cc) works when longer time
 *  periods need to be kept track of, to lower precision. 
 */

class task_timer
{
	protected:
		/// This time stamp object holds a most recently measured time
		time_stamp now_time;

	public:
		task_timer (void);					/// Constructor creates an empty timer
		void save_time_stamp (time_stamp&);	/// Save current time in a timestamp
		time_stamp& get_time_now (void);	/// Get the current time

		/// This method sets the current time to the time in the given time stamp
		bool set_time (time_stamp&);
};

//--------------------------------------------------------------------------------------
/// This operator allows timestamps to be written to serial ports 'cout' style
//base_text_serial& operator<< (base_text_serial&, time_stamp&);

#endif  // _STL_TIMER_H_
