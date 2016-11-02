//*************************************************************************************
/** \file task_menu.h
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MENU_H_
#define _MENU_H_

extern volatile int16_t button_presscount;

extern volatile unsigned char startTimelapse; 
extern volatile unsigned char init_left;
extern volatile unsigned char init_right;


extern void menuitem_eeprominit();
extern void menuitem_eepromread();
extern void menuitem_eepromwrite();

// extern void menuitem2_enter(void);
// extern void menuitem2_exit(void);
// extern void menuitem3_enter(void);
// extern void menuitem3_exit(void);

extern void menuitem1sub1_enter();
extern void menuitem1sub1_exit();
extern void menuitem1sub2_enter();
extern void menuitem1sub2_exit();
extern void menuitem1sub3_enter();
extern void menuitem1sub3_exit();
extern void menuitem1sub4_enter();
extern void menuitem1sub4_exit();
extern void menuitem1sub5_enter();
extern void menuitem1sub5_exit();

extern void menuitem2sub1_enter();
extern void menuitem2sub1_exit();
extern void menuitem2sub2_enter();
extern void menuitem2sub2_exit();
extern void menuitem2sub3_enter();
extern void menuitem2sub3_exit();

extern void menuitem3sub1_enter();
extern void menuitem3sub1_exit();
extern void menuitem3sub2_enter();
extern void menuitem3sub2_exit();

extern void menuitem4_enter(void);

extern void menuitem_select(void);
extern void menuitem_initialize();

extern unsigned char GetMotorRPM();
extern unsigned int GetStepsPerRev();
extern unsigned int GetTrackLength();
extern unsigned int GetPitch();
extern unsigned char GetTeeth();

extern unsigned char GetShutterSpeed();
extern unsigned char GetPicDelay();
extern unsigned char GetMotorDelay();
extern unsigned int GetTimelapsePeriod();


#endif

#ifdef __cplusplus
}
#endif

// following line turns on automatic (because I am lazy or smart, there is a fine line) indentation for Kate editor.
// kate: space-indent on; indent-width 5; mixedindent off; indent-mode cstyle;
