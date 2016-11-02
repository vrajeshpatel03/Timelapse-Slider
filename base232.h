//*************************************************************************************
/** \file base232.h
 *    This file contains a set of macros which are used by several classes and class
 *    templates that interact with the asynchronous (RS-232 style) serial port on an 
 *    AVR microcontroller. Classes which use this stuff include rs232 and packet232. 
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
 *
 *  License:
 *    This file is released under the Lesser GNU Public License, version 2. This 
 *    program is intended for educational use only, but it is not limited thereto. 
 */
//*************************************************************************************

/// This define prevents this .h file from being included more than once in a .cc file
#ifndef _BASE232_H_
#define _BASE232_H_

// #include "base_text_serial.h"				// Pull in the base class header file

// Check that the user has set the CPU frequency in the Makefile; if not, complain
#ifndef CPU_FREQ_Hz
	#error The macro CPU_FREQ_Hz must be set in the Makefile
#endif

//-------------------------------------------------------------------------------------
/** The number of tries to wait for the transmitter buffer to become empty */
#define UART_TX_TOUT		20000

//-------------------------------------------------------------------------------------
/** If this macro is defined, the UART will run in double-speed mode. This is often
 *  a good idea, as it allows higher baud rates to be used with not so high CPU clock
 *  frequencies. 
 */
#define UART_DOUBLE_SPEED

//-------------------------------------------------------------------------------------
/** This macro computes a value for the baud rate divisor from the desired baud rate 
 *  and the CPU clock frequency. The CPU clock frequency should have been set in the 
 *  macro CPU_FREQ_MHz, which is normally configured in the Makefile. The divisor is
 *  calculated as (frequency, MHz / (16 * baudrate)) - 1, unless the USART is running
 *  in double-speed mode, in which case the divisor is twice as big. 
 */

#ifdef UART_DOUBLE_SPEED
	#define calc_baud_div(baud_rate) \
		((uint8_t)((uint16_t)((CPU_FREQ_Hz * 20) / (16UL * baud_rate) - 5) / 20))
#else
	#define calc_baud_div(baud_rate) \
		((unsigned int)((CPU_FREQ_Hz * 10) / (16UL * baud_rate) - 5) / 20)
#endif

#endif  // _BASE232_H_
