//======================================================================================
/** \file  avr_adc.h
 *		This file contains the methods for the avr_adc class.  The main 
 *		functions of this class are read_once and read_several.
 *
 *  Revisions:
 *  \li  01-13-09  Initial Version Created
 *	\li  01-19-09  Version 0.9 Finalized
 *	\li  11-20-09  Changed to 6 channels for Atmega168
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#include <stdlib.h>							// Include standard library header files
#include <avr/io.h>							// You'll need this for SFR and bit names
#include <avr/interrupt.h>

#include "rs232.h"							// Include header for serial port class
#include "avr_adc.h"						// Include header for the A/D class

/** This method sets up some basic defaults and turns the ADC on.  It also runs
 *	the ADC once because the documentation says the first reading is bad.  This
 *	method is used to allow the programmer to easily change debug_me and
 *	time_out_cnts
 */
void avr_adc::init_me(void)
{
	debug_me=false;			// Set Debugging flag
	time_out_cnts=10000;	// Set time_out_cnts for time out counting
	ADCSRA |= (1<<ADEN); // Enables the ADC by writing a 1 to ADEN in ADCSRA
// 	ADCSRA |= (1<<ADSC);
	short temp;
	temp=read_raw (3);  // This just reads the ADC because the first reading is bad and takes longer
}

/** This constructor sets up an A/D converter, specifically one where no port
 *  is entered by the user.  This constructor will set the port to 0
 */
avr_adc::avr_adc (void)
{
	init_me();
	ptr_to_serial = 0;		// Set ptr_to_serial to 0
	my_ref=5.00;			// Set reference voltage
}

/** This constructor sets up an A/D converter with a port to write to for
 *  debugging purposes.  It also initializes the number of counts for time out
 *	and the reference voltage to be used.
 *  @param p_serial_port A pointer to the serial port which writes debugging info. 
 */
avr_adc::avr_adc (base_text_serial* p_serial_port)
{
	init_me();
	ptr_to_serial = p_serial_port;          // Store the serial port pointer locally
	my_ref=5.00;			// Set reference voltage
	init_me();

	// Output intro text
	*ptr_to_serial << endl <<"Setting up AVR A/D converter\r";
}


/** This constructor sets up an A/D converter and provides a port to write to for
 *	debugging, allows the user to change the clock divisor and the way the ADC
 *	uses its' reference voltage.
 *  @param p_serial_port A pointer to the serial port which writes debugging info. 
 *	@param clock_div Lets the user select clock divisor, must be 2,4,8,16,32,64,128
 *	@param v_type Lets the user select the voltage reference, 0 for 5V, 1 for User applied Vcc, 2 for internal 2.56V
 *	@param v_ref If the user selects v_type of 1, they must provide a floating point refernece V
 */
avr_adc::avr_adc (base_text_serial* p_serial_port, unsigned char clock_div, char v_type, float v_ref)
{
	init_me();
	ptr_to_serial = p_serial_port;	// Store the serial port pointer locally

	// Note that when the serial port pointer is used to send a message, it must be
	// dereferenced with a "*", meaning "what is pointed to by ptr_to_serial" 
	*ptr_to_serial << "Setting up AVR A/D converter\r";

	// This code enables the ADC and sets the clock speed and V_ref
	ADCSRA |= (1<<ADEN); // Enables the ADC by writing a 1 to ADEN
// 	ADCSRA |= (1<<ADSC);
	// This code controls the setup of V_ref
	if (v_type==0)
	{
		// User wants to keep default AREF, internal V_ref turned off
		my_ref=5.00;
	}
	else if(v_type==1)
	{
		// User wants to use AVcc with external capacitor at AREF pin
		// User must include a v_ref to use this option
		ADMUX |= (1<<REFS0);	// Write a 1 to REFS0 to select the correct mode
		my_ref = v_ref;			// Set my_ref to v_ref for use in multiplication
	}
	else if(v_type==2)
	{
		// User wants to use the internal 2.56 voltage reference with an exteranl capacitor at the AREF pin
		ADMUX |= (1<<REFS1);	// Write a 1 to REFS 1 and REFS 0 to select this mode
		ADMUX |= (1<<REFS0);
		my_ref = 2.56;			// Set my_ref
	}
	else
	{
		my_ref=5.00;
		*ptr_to_serial << "Invalid reference voltage type used: default of 5.00V used\r";
	}

	//This code sets up the clock speed
	if (clock_div==2)
	{
		//Do nothing, this is the default
	}
	else if (clock_div==4)
	{
		ADCSRA |= (1<<ADPS1);
	}
	else if (clock_div==8)
	{
		ADCSRA |= (1<<ADPS1);
		ADCSRA |= (1<<ADPS0);
	}
	else if (clock_div==16)
	{
		ADCSRA |= (1<<ADPS2);
	}
	else if (clock_div==32)
	{
		ADCSRA |= (1<<ADPS2);
		ADCSRA |= (1<<ADPS0);
	}
	else if (clock_div==64)
	{
		ADCSRA |= (1<<ADPS2);
		ADCSRA |= (1<<ADPS1);
	}
	else if (clock_div==128)
	{
		ADCSRA |= (1<<ADPS2);
		ADCSRA |= (1<<ADPS1);
		ADCSRA |= (1<<ADPS0);
	}
	else
	{
		//Invalid Number
		*ptr_to_serial << "Invalid Clock Divisor used: default of 2 used." << endl;
	}
}

/** This method turns off the ADC to save power
 */
void avr_adc::adc_off (void)
{
	ADCSRA &= (~(1<<ADEN));	// The math is, bitshift a 1 into ADEN (everything else 0), ~ performs a binary not,
							// so the bits invert, and then we AND this new number with ADCSRA to clear the ADEN
							// bit but not affect the other bits
}

/** This method turns the ADC back on if the user turned it off
 */
void avr_adc::adc_on (void)
{
	ADCSRA |= (1<<ADEN); // Enables the ADC by writing a 1 to ADEN
}

/** This method takes one A/D reading from the given channel, and...?
 *  @return my_ref
 */
float avr_adc::get_vref(void)
{
	return my_ref;
}

/** This method takes one A/D reading from the given channel, and returns it.
 *	It also outputs debugging info.
 *  @param  channel The A/D channel which is being read must be from 0 to 7
 *  @return The result of the A/D conversion, or -1 if there was a timeout or error
 */
short avr_adc::read_raw (unsigned char channel)
{
	//First check to see that channel is valid
	if (channel>6)
	{
		if (debug_me)
		{
			*ptr_to_serial << "Read_raw, invalid channel: " << channel<<"\r";
		}
		return -1;
	}

	// Declare some variables to use
	unsigned char 	low	= 0;		// Will hold the low byte of data of the reading
	unsigned char 	high= 0;		// Will hold the high byte of the of the reading
	short 			total=0;		// Will hold the total byte reading
	volatile unsigned short cnt_time=0;	// This counts to check for ADC timeout

	// Now choose which channel to read and run the ADC
	ADMUX &= 0b11110000; 	// Initialize ADMUX by clearing out the last 4 bits
	ADMUX |= channel; 		// Place the channel into ADMUX to read the proper channel, the binary math lines up
	ADCSRA |= (1<<ADSC); 	// This command tells ADC to run
	//ADCSRA |= (1<<ADSC);
	
	// Wait for the ADC to tell us it is ready, and then, store the reading into total
	while (!(ADCSRA & (1<<ADIF))) // Keeps checking to see if the ADC is done running
	{
		// If the ADC takes too long, it has timed out, report such
		if (++cnt_time>time_out_cnts)
		{
			*ptr_to_serial << "Read_raw, ADC time out, channel: " << channel << "\r";
			return -1;
		}
	}

	low		= ADCL;			// First read in the low byte.  Cannot read high byte until this is done.
	high	= ADCH;			// Read the high byte
	high 	&= 0b00000011;	// This ensures that no random data in the high byte gets through
	total	= high<<8;		// Place the high byte into the high part of total using bitshift
	total	+=low;			// Add in the low byte

	ADCSRA |= (1<<ADIF); // Now tell the interrupt to put its hand down, not sure if this is necessary, better safe than sorry
	
	// For now, just send a message saying this method is running properly
	if (debug_me)
	{
		*ptr_to_serial << "Read Raw, channel: "<<channel<<", reads: " << total << "\r";
	}

	// Return the total!
	return total;
}

/** This method takes one A/D reading from the given channel, lets the user 
 *	filter it somehow, and returns the resulting digital number.
 *  @param  channel The A/D channel which is being read must be from 0 to 7
 * 	@param	filter_num is the type of filter the user would like to use: 0 means no filter, 1 means average 10 readings
 *  @return The result of the A/D conversion, or -1 if there was an error
 */
short avr_adc::read_once (unsigned char channel, unsigned char filter_num)
{
	unsigned short 	results		= 0; 	// Holds sum of results
	short 			temp		= 0; 	// Grabs result from read_raw, so the program can test to see if it is -1
	unsigned char 	success		= 0;	// This counts the number of successfully 
										// readings.  If 1 bad reading is performed, the rest will still be averaged
	unsigned char 	num_avgs	= 10;	// Set this to tell the program how many readings you want to average
	unsigned char 	i			= 0;	// Counter for averaging
	
	if (filter_num==0)
	{
		// Do nothing, program will fall through and run the default.
	}
	else if (filter_num==1)
	{
		// Run a filter of averaging by calling read_raw num_avgs times.
		for(i=0;i<num_avgs;i++)
		{	
			temp=read_raw(channel);  // First read into a temp, so we can check for errors
			if (temp>=0)
			{
				results+=temp;	// Add to the sum of results
				success++;		// Increment success so we know what to divide by
			}
			else
			{
				// Do nothing
			}
		}
		
		if (success==0)
		{	
			// All the runs were bad
			if (debug_me)
			{
				*ptr_to_serial << "Read Once, channel: "<<channel<<", has an error in averaging filter\r";
			}
			return -1;	
		}
		
		temp=results/success;	// Calculate the average.  Integer math is okay, since we are looking at 0 to 1023, with low precision.
		return temp;
	}
	else
	{
		if (debug_me)
		{
			*ptr_to_serial << "Read_once, unknown filter type, channel: "<<channel<<", proceeding to read with no filter"<< "\r";
		}
	}
	
	return read_raw(channel); 	// Default reading
}

/**	This function will take in several channels and get readings from the ADC
 *	using the filter_num provided.
 *  @param	num is the number of channels
 *	@param	channels is an array of channels to read
 *	@param	readings is the array of readings to be returned
 * 	@param	filter_num is the type of filter the user would like to use
 *	@return	Nothing is returned, data is returned in readings
 */
void avr_adc::read_several (unsigned char num, unsigned char channels[],short readings[], unsigned char filter_num)
{
	unsigned char i=0;	// Initialize characters
	short temp;			// Temporary storage for readings

	//First make sure num is less than 8, since there are only 8 channels
	if (num>8) 
	{
		if (debug_me)
		{
			*ptr_to_serial<<"Read_several tried too read many channels"<<"\r";  // Maybe should fill readings in with -1's or something?
		}
		return;
	}
	
	//Now run through the array of channels desired and use read_once to get a reading
	for (i=0;i<num;i++)
	{
		temp = read_once (channels[i],filter_num);	// First read once into temp, and check for errors
		if (temp>=0)
		{
			// If the reading is valid
			readings[i] = temp;	// Store the temporary reading into the array
			if (debug_me)
			{
				*ptr_to_serial<<"Read_several, channel: "<<channels[i]<<" shows: "<<readings[i]<<"\r";
			}
		}
		else
		{
			readings[i] = -1; // Bad reading, store -1
			if (debug_me)
			{
				*ptr_to_serial<<"Read_several, channel: "<<channels[i]<<" encountered an error \r";
			}
		}
	}
}

/** This operator prints an A/D converter on a text serial device. You may decide what
 *  is most useful to print; it should be some information about how the A/D converter
 *  is set up, what it's doing, current readings, or something like that. 
 */
base_text_serial& operator<< (base_text_serial& serial, avr_adc& my_adc)
{
	unsigned char 	my_num=3; 				// Number of channels to read
	unsigned char 	my_channels[]={3,4,5};	// List of channels to read
	short 			my_readings[]={0,0,0};	// Array to hold results
	unsigned char 	my_filter=0; 			// Number for which filter to use
	unsigned char 	i=0;					// Counter
	
	my_adc.read_several(my_num,my_channels,my_readings,my_filter);	// Call read several
	
	// This prints the readings from above
	serial << "Current voltages: ";
	for(i=0;i<my_num;i++)
	{
		serial << "chan: "<<my_channels[i]<<", volt: "<<my_readings[i];
		if (i<(my_num-1))
		{
			serial<<"; ";
		}
	}
	serial<<"\r";

	// This statement should be left here; you must return a reference to the serial
	// device so that the "<<" operator can be used again and again on the same line
	return (serial);
}
