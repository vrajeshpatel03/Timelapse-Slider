//======================================================================================
/** \file  avr_adc.h
 *	Class Header File: This class is a device driver for the Analog to Digital
 *	Converter on an AVR processor.
 *
 *  Revisions:
 *    \li  01-13-09  Initial Program created
 *	  \li  01-19-09  Version 0.9 finalized
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

/// This define prevents this .h file from being included more than once in a .cc file
#ifndef _AVR_ADC_H_
#define _AVR_ADC_H_

//-------------------------------------------------------------------------------------
/** This class runs the A/D converter on an AVR processor. The main methods to 
 *	use are read_once and read_several.  Please refer to the individual 
 *	functions for a better explanation of each method.
 */
class avr_adc
{
	protected:
		/**
		 *	ptr_to_serial stores a reference to a serial port to write to
		 */
		base_text_serial* ptr_to_serial;	// This stores the serial port address to write to
		/**
		 *	my_ref stores a reference voltage for a program to use to get the analog reading from the ADC
		 */
		float my_ref;						// This stores the reference voltage for a program using the ADC to multiply
		/**
		 *	time_out_cnts stores a number to count to for measuring timeout of the ADC
		 */
		unsigned short time_out_cnts; 		// The ADC will be classified as taking to long if it gets to this many cnts
		/**
		 *	debug_me is a bool.  True to output debugging info, false to not.
		 */
		bool debug_me; 						// True for debug text on, false for debug text off
		short read_raw (unsigned char);		// Make a reading, this is "raw" because other functions will call it and apply filters
		void init_me(void); // This will be called by constructors to easily perform some tasks they all must perform

	public:
		avr_adc (void); 				// This constructor will set the serial port pointer to 0, leaves settings at default and enables ADC
		avr_adc (base_text_serial*);	// This constructor sets up a serial port pointer, leaves settings at default and enables ADC
		// This constructor sets up a serial port pointer, clock divisor for prescalar, and a reference voltage
		avr_adc (base_text_serial*, unsigned char, char, float); 
		
		void adc_off (void); 	//This function will turn off the ADC to save power
		void adc_on (void); 	//This function will turn on the ADC if it was turned off
		float get_vref(void);	// Pass V_ref so another function can calculate analog voltage from digital readings
		
		// This function will read the ADC for the user or for read several.
		// It requires a channel and a filter number to read.
		short read_once (unsigned char, unsigned char);

		// This function to reads a set of several channels into an array.
		// The parameters are the number of channels, an array of channels,
		// an array of shorts to store the results, and the filter to use.
		void read_several (unsigned char, unsigned char[],short[], unsigned char);
    };

/** This operator prints information from an A/D converter on a text serial 
 *	device. It will print the readings from channels 3 through 5.
 */
base_text_serial& operator<< (base_text_serial&, avr_adc&);

#endif // _AVR_ADC_H_
