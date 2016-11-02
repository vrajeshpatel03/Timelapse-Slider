//*************************************************************************************
/** \file rs232.cc
 *    This file contains a class which allows the use of a serial port on an AVR 
 *    microcontroller. The port is used in "text mode"; that is, the information which 
 *    is sent and received is expected to be plain ASCII text, and the set of 
 *    overloaded left-shift operators "<<" in base_text_serial.* can be used to easily
 *    send all sorts of data to the serial port in a manner similar to iostreams 
 *    (think of "cout") in regular C++. 
 *
 *  Revised:
 *    \li 04-03-2006 JRR For updated version of compiler
 *    \li 06-10-2006 JRR Ported from C++ to C for use with some C-only projects; also
 *        serial_avr.h now has defines for compatibility among lots of AVR variants
 *    \li 08-11-2006 JRR Some bug fixes
 *    \li 03-02-2007 JRR Ported back to C++. I've had it with the limitations of C.
 *    \li 04-16-2007 JO  Added write (unsigned long)
 *    \li 07-19-2007 JRR Changed some character return values to bool, added m324p
 *    \li 01-12-2008 JRR Added code for the ATmega128 using USART number 1 only
 *    \li 02-14-2008 JRR Split between base_text_serial and rs232 files
 *    \li 05-31-2008 JRR Changed baud calculations to use CPU_FREQ_MHz from Makefile
 *    \li 06-01-2008 JRR Added getch_tout() because it's needed by 9Xstream modems
 *    \li 07-05-2008 JRR Changed from 1 to 2 stop bits to placate finicky receivers
 *    \li 12-22-2008 JRR Split off stuff in base232.h for efficiency
 *	 \li 11-20-2009 VP	Added support for ATmega168 (arduino) 
 *
 *  License:
 *		This file is released under the Lesser GNU Public License, version 2. This 
 *		program is intended for educational use only, but it is not limited thereto. 
 */
//*************************************************************************************

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include "rs232.h"


//-------------------------------------------------------------------------------------
/** This method sets up the AVR UART for communications.  It enables the appropriate 
 *  inputs and outputs and sets the baud rate divisor, and it saves pointers to the
 *  registers which are used to operate the serial port. Since some AVR processors
 *  have dual serial ports, this method allows one to specify a port number. 
 *  @param baud_rate The desired baud rate for serial communications. Default is 9600
 *  @param port_number The number of the serial port, 0 or 1 (the second port numbered
 *                     1 only exists on some processors). The default is port 0 
 */

rs232::rs232 (unsigned int baud_rate, unsigned char port_number)
	: base_text_serial ()
{
	if (port_number == 0)
	{
	#ifdef __AVR_AT90S2313__
		p_UDR = &UDR;
		p_USR = &USR;
		p_UCR = &UCR;
		UCR = (1 << RXEN) | (1 << TXEN);		// 0x18 for mode N81
		UBRR = calc_baud_div (baud_rate);
		mask_UDRE = (1 << UDRE);
		mask_RXC = (1 << RXC);
		mask_TXC = (1 << TXC);
	#endif
// 	#if defined __AVR_ATmega168__
// 		p_UDR = &UDR0;
// 		p_USR = &UCSR0A;
// 		p_UCR = &UCSR0B;
// 		UCSR0B = 
// 	#endif
	#if defined __AVR_ATmega8__ || defined __AVR_ATmega8535__ \
		|| defined __AVR_ATmega32__
		p_UDR = &UDR;
		p_USR = &UCSRA;
		p_UCR = &UCSRB;
		UCSRB = (1 << RXEN) | (1 << TXEN);
		UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0); // | (1 << USBS0);
		UBRRH = 0x00;
		UBRRL = calc_baud_div (baud_rate);
		#ifdef UART_DOUBLE_SPEED				// Activate double speed mode
			UCSRA |= U2X;						// if required
		#endif
		mask_UDRE = (1 << UDRE);
		mask_RXC = (1 << RXC);
		mask_TXC = (1 << TXC);
	#endif
	#if defined __AVR_ATmega644__ || defined __AVR_ATmega324P__ \
		|| defined __AVR_ATmega128__ || defined __AVR_ATmega168__ || defined __AVR_ATmega2560__
		p_UDR = &UDR0;
		p_USR = &UCSR0A;
		p_UCR = &UCSR0B;
		UCSR0B = (1 << RXEN0) | (1 << TXEN0);
		UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (1 << USBS0);
		UBRR0H = 0x00;
		UBRR0L = calc_baud_div (baud_rate);
		#ifdef UART_DOUBLE_SPEED				// Activate double speed mode
			UCSR0A |= U2X0;						// if required
		#endif
		mask_UDRE = (1 << UDRE0);
		mask_RXC = (1 << RXC0);
		mask_TXC = (1 << TXC0);
	#endif
	}
	else
	{ // Some AVR's only have one serial port (losers)
	#if defined __AVR_AT90S2313__ || defined __AVR_ATmega8__ \
		|| defined __AVR_ATmega8535__ || defined __AVR_ATmega32__
		p_UDR = NULL;
		p_USR = NULL;
		p_UCR = NULL;
		mask_UDRE = 0;
		mask_RXC = 0;
		mask_TXC = 0;
	#endif
	#if defined __AVR_ATmega644__ || defined __AVR_ATmega324P__ \
		|| defined __AVR_ATmega128__
		p_UDR = &UDR1;
		p_USR = &UCSR1A;
		p_UCR = &UCSR1B;
		UCSR1B = (1 << RXEN1) | (1 << TXEN1);
		UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); // | (1 << USBS1);
		UBRR1H = 0x00;
		UBRR1L = calc_baud_div (baud_rate);
		#ifdef UART_DOUBLE_SPEED	// If double-speed macro has been defined,
			UCSR1A |= U2X1;			// turn on double-speed operation
		#endif
		mask_UDRE = (1 << UDRE1);
		mask_RXC = (1 << RXC1);
		mask_TXC = (1 << TXC1);
	#endif
	}

	// Read the data register to ensure that it's empty
	port_number = *p_UDR;
	port_number = *p_UDR;
}


//-------------------------------------------------------------------------------------
/** This method checks if the serial port transmitter is ready to send data.  It 
 *  tests whether transmitter buffer is empty. 
 *  @return True if the serial port is ready to send, and false if not
 */

bool rs232::ready_to_send (void)
{
	// If transmitter buffer is full, we're not ready to send
	if (*p_USR & mask_UDRE)
		return (true);

	return (false);
}


//-------------------------------------------------------------------------------------
/** This method checks if the serial port is currently sending a character. Even if the
 *  buffer is ready to accept a new character, the port might still be busy sending the
 *  last one; it would be a bad idea to put the processor to sleep before the character
 *  has been sent. 
 *  @return True if the port is currently sending a character, false if it's idle
 */

bool rs232::is_sending (void)
{
	if (*p_USR & mask_TXC)
		return (false);
	else
		return (true);
}

		
//-------------------------------------------------------------------------------------
/** This method sends one character to the serial port.  It waits until the port is
 *  ready, so it can hold up the system for a while.  It times out if it waits too 
 *  long to send the character; you can check the return value to see if the character
 *  was successfully sent, or just cross your fingers and ignore the return value.
 *  Note 1:  It's possible that at slower baud rates and/or higher processor speeds, 
 *  this routine might time out even when the port is working fine.  A solution would
 *  be to change the count variable to an integer and use a larger starting number. 
 *  Note 2:  Fixed!  The count is now an integer and it works at lower baud rates.
 *  @param chout The character to be sent out
 *  @return True if everything was OK and false if there was a timeout
 */

bool rs232::putchar (char chout)
{
	// Now wait for the serial port transmitter buffer to be empty	 
	for (unsigned int count = 0; ((*p_USR & mask_UDRE) == 0); count++)
	{
		if (count > UART_TX_TOUT)
			return (false);
	}

	// Clear the TXCn bit so it can be used to check if the serial port is busy.  This
	// check needs to be done prior to putting the processor into sleep mode.  Oddly,
	// the TXCn bit is cleared by writing a one to its bit location
	*p_USR |= mask_TXC;

	// The CTS line is 0 and the transmitter buffer is empty, so send the character
	*p_UDR = chout;
	return (true);
}


//-------------------------------------------------------------------------------------
/** This method writes all the characters in a string until it gets to the '\\0' at 
 *  the end. Warning: This function blocks until it's finished. 
 *  @param str The string to be written 
 */

void rs232::puts (char const* str)
{
	while (*str) putchar (*str++);
}


//-------------------------------------------------------------------------------------
/** This method gets one character from the serial port, if one is there.  If not, it
 *  waits until there is a character available.  This can sometimes take a long time
 *  (even forever), so use this function carefully.  One should almost always use
 *  check_for_char() to ensure that there's data available first. 
 *  @return The character which was found in the serial port receive buffer
 */

char rs232::getchar (void)
{
	//  Wait until there's something in the receiver buffer
	while ((*p_USR & mask_RXC) == 0);

	//  Return the character retreived from the buffer
	return (*p_UDR);
}


//-------------------------------------------------------------------------------------
/** This method checks if there is a character in the serial port's receiver buffer.
 *  It returns 1 if there's a character available, and 0 if not. 
 *  @return True for character available, false for no character available
 */

bool rs232::check_for_char (void)
{
	if (*p_USR & mask_RXC)
		return (true);
	else
		return (false);
}


//-------------------------------------------------------------------------------------
/** This method tries the given number of times to get a character from the receiver
 *  buffer. It returns the received character, or 0xFF if there was a timeout.
 *  @param retries The number of times to attempt to get a character
 *  @return The character received, if one was received; otherwise 0xFF
 */

char rs232::getch_tout (unsigned int retries)
{
	for (unsigned int count = 0; count < retries; count++)
	{
		if (*p_USR & mask_RXC)
			return (*p_UDR);
	}
	return (0xFF);
}
