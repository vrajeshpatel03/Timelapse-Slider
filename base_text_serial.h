//*************************************************************************************
/** \file base_text_serial.h
 *    This file contains a base class for devices which send information in text form
 *    over serial devices in an AVR microcontroller. Example devices are serial ports
 *    (both traditional RS-232 ports and USB-serial adapters) and wireless modems. 
 *
 *    This code is designed to work for low-performance applications without requiring
 *    the use of interrupts.  Interrupt based receiving code has not been completed or 
 *    tested.
 *
 *  Revised:
 *    \li 04-03-2006 JRR For updated version of compiler
 *    \li 06-10-2006 JRR Ported from C++ to C for use with some C-only projects
 *    \li 08-11-2006 JRR Some bug fixes
 *    \li 03-02-2007 JRR Ported back to C++. I've had it with the limitations of C.
 *    \li 04-16-2007 JO  Added write (unsigned long)
 *    \li 07-19-2007 JRR Changed some character return values to bool, added m324p
 *    \li 01-12-2008 JRR Added code for the ATmega128 using USART number 1 only
 *    \li 02-13-2008 JRR Split into base class and device specific classes; changed
 *                       from write() to overloaded << operator in the "cout" style
 *    \li 05-12-2008 ALS Fixed bug in which signed numbers came out unsigned
 *    \li 07-05-2008 JRR Added configuration macro by which to change what "endl" is
 *    \li 07-05-2008 JRR Added 'ascii' and 'numeric' format codes
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto. 
 */
//*************************************************************************************

/// This define prevents this .h file from being included more than once in a .cc file
#ifndef _BASE_TEXT_SERIAL_H_
#define _BASE_TEXT_SERIAL_H_


/** \brief This define selects what will be sent when a program sends out "endl".
 *  Different recieving programs want different end-of-line markers. Traditionally, 
 *  UNIX uses "\r" while PC's use "\r\n" and Macs use "\n" (I think). */
#define ENDL_STYLE	  "\r\n"

/** \brief This define selects the character which asks a terminal to clear its screen.
 *  This is usually a control-L, which is ASCII character number 12.
 */

#define CLRSCR_STYLE	((unsigned char)12)


//-------------------------------------------------------------------------------------
/** This enumeration is used to change the display base for the output stream from the
 *  default of 10 (decimal) to and from 2 (binary), 8 (octal), or 16 (hexadecimal).
 *  Also defined are endl for end-of-line and send_now to tell packet based devices to
 *  transmit a packet as soon as possible. The base is changed for the next printing 
 *  of one number only, then the base reverts to the default value of base 10. Codes
 *  are also provided to allow printing of 8-bit numbers as ASCII characters and to 
 *  tell some devices to send packets of data immediately. 
 */

typedef enum {
	bin,					///< Print following numbers in base 2 (binary)
	oct,					///< Print following numbers in base 8 (octal)
	dec,					///< Print following numbers in base 10 (decimal)
	hex,					///< Print following numbers in base 16 (hexadecimal)
	ascii,					///< Print an ASCII character rather than an 8-bit number
	numeric,				///< Print 8-bit numbers as numeric values, not ASCII
	endl,					///< Print a carriage return and linefeed
	clrscr,					///< Send a control-L which clears some terminal screens
	send_now				///< Tell some radio devices to send data immediately
} ser_manipulator;


//-------------------------------------------------------------------------------------
/** This is a base class for lots of serial devices which send text over some type of
 *  communication interface. Descendents of this class will be able to send text over
 *  serial lines, radio modems or modules, and whatever other communication devices
 *  might be appropriate. 
 *
 *  The most important methods of this class are the overloaded "<<" operators which 
 *  convert different types of numbers into printable strings, then print them using
 *  methods which are overloaded by descendent classes. The descendent classes know
 *  how to actually send out characters. Methods which will be inherited or overridden 
 *  by descendents include the following: 
 *    \li ready_to_send () - Checks if the port is ready to transmit a character
 *    \li putchar() - Sends a single character over the communications line
 *    \li puts() - Sends a character string
 *    \li operator<<() - Methods which convert numbers to strings and send them
 */

class base_text_serial
{
	// Private data and methods are accessible only from within this class and 
	// cannot be accessed from outside -- even from descendents of this class
	private:

	// Protected data and methods are accessible from this class and its descendents
	// only
	protected:
		/// This is the currently used base for converting numbers to text
		unsigned char base;
		
		/// If this variable is set true, print characters as ASCII characters, not
		/// as numbers
		bool print_ascii;

	// Public methods can be called from anywhere in the program where there is a 
	// pointer or reference to an object of this class
	public:
		base_text_serial (void);			// Simple constructor doesn't do much
		virtual bool ready_to_send (void);  // Virtual and not defined in base class
		virtual bool putchar (char) { }	 ///< Virtual and not defined in base class
		virtual void puts (char const*) { } ///< Virtual and not defined in base class
		virtual bool check_for_char (void); // Check if a character is in the buffer
		virtual char getchar (void);		// Get a character; wait if none is ready
		virtual void transmit_now (void);   // Immediately transmit any buffered data

		// The overloaded left-shift operators convert numbers to strings and send the 
		// strings out the serial device; manipulators change the formatting
		base_text_serial& operator<< (bool);
		base_text_serial& operator<< (const char*);
		base_text_serial& operator<< (unsigned char);
		base_text_serial& operator<< (char num);
		base_text_serial& operator<< (unsigned int);
		base_text_serial& operator<< (int);
		base_text_serial& operator<< (unsigned long);
		base_text_serial& operator<< (long);
		base_text_serial& operator<< (unsigned long long);
		base_text_serial& operator<< (ser_manipulator);
};

#endif  // _BASE_TEXT_SERIAL_H_
