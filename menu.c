//*************************************************************************************
/** \file menu.cc
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
 *   	\li  06-07-2015     VP		Initial Version created
 *   	\li  11-14-2015	 	VP		Created working menu. Need to edit limits.
 *		\li  01-31-2016		VP		Worked on defining limits 	
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto. 
 */
//*************************************************************************************
#define F_CPU 16000000UL

#include <stdlib.h>             //standard avr library
#include <avr/io.h>             //standard avr io library
#include <string.h>				//standard library
#include <avr/interrupt.h>		//standard library
#include <avr/eeprom.h>			//standard library
#include <avr/pgmspace.h>		//standard library	
#include <util/delay.h>			//standard library



#include "lcd.h"
#include "micromenu.h"
#include "lcdmenu1.h"

#include "menu.h"

//def int number of buttons
#define BUTTON_NUM 5

//define buttons
#define BUTTON_RIGHT 0
#define BUTTON_UP 1
#define BUTTON_DOWN 2
#define BUTTON_LEFT 3
#define BUTTON_SELECT 4

//define adc read max values for button input
#define BUTTON_ADCRIGHT 50
#define BUTTON_ADCUP 250
#define BUTTON_ADCDOWN 350
#define BUTTON_ADCLEFT 550
#define BUTTON_ADCSELECT 850

//button channel
#define BUTTON_CHANNEL 5

//button press delay, prevent multiple keypress
#define BUTTON_PRESSDELAYMS 300

//button repetition counter
#define BUTTON_PRESSCOUNTMAX10 10
#define BUTTON_PRESSCOUNTMAX100 20
#define BUTTON_PRESSCOUNTMAX 20 //max between above definitions

//volatile int16_t button_presscount = 0;


//lcd buffer line
static char lcdbuff[16];

//define the eeprom structure
typedef struct 
{
	uint8_t initeeprom;
	unsigned char motorRPM;
	unsigned int stepsPerRev;
	unsigned int trackLength;
	unsigned int pitch;
	unsigned char teeth;
	unsigned char shutterSpeed;
	unsigned char picDelay;
	unsigned char motorDelay;
	unsigned int timelapsePeriod;
} menuitem_eet;

menuitem_eet EEMEM menuitem_eemem; //move this to task_menu
menuitem_eet menuitem_eevar;

//-------------------------------------------------------------------------------------
/*
 * init menu eeprom
 */
void menuitem_eeprominit() 
{
	//Initial values
	menuitem_eevar.initeeprom = 1;
	menuitem_eevar.motorRPM = 30;
	menuitem_eevar.stepsPerRev = 200;
	menuitem_eevar.trackLength = 1800;
	menuitem_eevar.pitch = 2032;
	menuitem_eevar.teeth = 18;
	menuitem_eevar.shutterSpeed = 20;
	menuitem_eevar.picDelay = 1;
	menuitem_eevar.motorDelay = 1;
	menuitem_eevar.timelapsePeriod = 300;
		
	//Write the initial values to EEPROM
	eeprom_write_block((const void*)&menuitem_eevar, (void*)&menuitem_eemem, sizeof(menuitem_eet));
}


//-------------------------------------------------------------------------------------
/*
 * read menu eeprom
 */
void menuitem_eepromread() 
{
	eeprom_read_block((void*)&menuitem_eevar, (const void*)&menuitem_eemem, sizeof(menuitem_eet));
}


//-------------------------------------------------------------------------------------
/*
 * write menu eeprom
 */
void menuitem_eepromwrite()
{
	eeprom_write_block((const void*)&menuitem_eevar, (void*)&menuitem_eemem, sizeof(menuitem_eet));
}


//-------------------------------------------------------------------------------------
/*
 * menu edit functions
 */
// #define lcdcursor_POSEDITINIT 0
// int menuitem2_val = 0;
// #define MENUITEM2_MAX 1000
// #define MENUITEM2_MIN 10

// void menuitem2_enter(void) {
	// if(!lcdmenu1_isediting()) {
		// menuitem_eepromread();
		// menuitem2_val = menuitem_eevar.menuitem2_val;
	// }
	// if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) {
		// if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			// menuitem2_val += 100;
		// else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			// menuitem2_val += 10;
		// else
			// menuitem2_val++;
	// } else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) {
		// if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			// menuitem2_val -= 100;
		// else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			// menuitem2_val -= 10;
		// else
			// menuitem2_val--;
	// }
	// if(menuitem2_val < MENUITEM2_MIN)
		// menuitem2_val = MENUITEM2_MIN;
	// if(menuitem2_val > MENUITEM2_MAX)
		// menuitem2_val = MENUITEM2_MAX;
	// itoa(menuitem2_val, lcdbuff, 10);
	// lcdmenu1_writebuff(lcdbuff);
	// lcd_gotoxy(lcdcursor_POSEDITINIT,1);
// }


//-------------------------------------------------------------------------------------
// void menuitem2_exit(void) 
// {
	// if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) {
		// menuitem_eevar.menuitem2_val = menuitem2_val;
		// menuitem_eepromwrite();
	// }
// }


//-------------------------------------------------------------------------------------
// uint8_t menuitem3_val = 0;

// void menuitem3_enter(void) 
// {
	// if(!lcdmenu1_isediting()) {
			// menuitem_eepromread();
			// menuitem3_val = menuitem_eevar.menuitem3_val;
		// }
	// if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) {
		// menuitem3_val = !menuitem3_val;
	// } else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) {
		// menuitem3_val = !menuitem3_val;
	// }
	// itoa(menuitem3_val, lcdbuff, 10);
	// lcdmenu1_writebuff(lcdbuff);
	// lcd_gotoxy(lcdcursor_POSEDITINIT,1);
// }


//-------------------------------------------------------------------------------------
// void menuitem3_exit(void) 
// {
	// if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) {
		// menuitem_eevar.menuitem3_val = menuitem3_val;
		// menuitem_eepromwrite();
	// }
// }


//----------Menu 1: Preferences---------------
 #define lcdcursor_POSEDITINIT 0
 
//Motor RPM
unsigned char motorRPM = 0;
#define MOTORRPM_MAX 200
#define MOTORRPM_MIN 7
void menuitem1sub1_enter(void)
{
	//Save variable to eeprom if menu editing is done
	if(!lcdmenu1_isediting()) 
	{
		menuitem_eepromread();
		motorRPM = menuitem_eevar.motorRPM;
	}
	
	//Pressing up button to increase value
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			motorRPM += 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			motorRPM += 10;
		else
			motorRPM++;
	} 
	//Pressing down button will decrease value
	else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			motorRPM -= 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			motorRPM -= 10;
		else
			motorRPM--;
	}
	
	if(motorRPM < MOTORRPM_MIN)
		motorRPM = MOTORRPM_MIN;
	if(motorRPM > MOTORRPM_MAX)
		motorRPM = MOTORRPM_MAX;
	itoa(motorRPM, lcdbuff, 10);
	lcdmenu1_writebuff(lcdbuff);
	lcd_gotoxy(lcdcursor_POSEDITINIT,1);
}

void menuitem1sub1_exit(void)
{
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) {
		menuitem_eevar.motorRPM = motorRPM;
		menuitem_eepromwrite();
	}
}


//Motor Steps/Rev
unsigned int stepsPerRev = 0;
#define STEPSPERREV_MAX 65535
#define STEPSPERREV_MIN 1
void menuitem1sub2_enter(void)
{
	//Save variable to eeprom if menu editing is done
	if(!lcdmenu1_isediting()) 
	{
		menuitem_eepromread();
		stepsPerRev = menuitem_eevar.stepsPerRev;
	}
	
	//Pressing up button to increase value
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			stepsPerRev += 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			stepsPerRev += 10;
		else
			stepsPerRev++;
	} 
	//Pressing down button will decrease value
	else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			stepsPerRev -= 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			stepsPerRev -= 10;
		else
			stepsPerRev--;
	}
	
	if(stepsPerRev < STEPSPERREV_MIN)
		stepsPerRev = STEPSPERREV_MIN;
	if(stepsPerRev > STEPSPERREV_MAX)
		stepsPerRev = STEPSPERREV_MAX;
	itoa(stepsPerRev, lcdbuff, 10);
	lcdmenu1_writebuff(lcdbuff);
	lcd_gotoxy(lcdcursor_POSEDITINIT,1);
}

void menuitem1sub2_exit(void)
{
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) {
		menuitem_eevar.stepsPerRev = stepsPerRev;
		menuitem_eepromwrite();
	}
}


//Track length (mm)
unsigned int trackLength = 0;
#define TRACKLENGTH_MAX 7000  // equals to 22.966 ft
#define TRACKLENGTH_MIN 10
void menuitem1sub3_enter(void)
{
	//Save variable to eeprom if menu editing is done
	if(!lcdmenu1_isediting()) 
	{
		menuitem_eepromread();
		trackLength = menuitem_eevar.trackLength;
	}
	
	//Pressing up button to increase value
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			trackLength += 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			trackLength += 10;
		else
			trackLength++;
	} 
	//Pressing down button will decrease value
	else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			trackLength -= 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			trackLength -= 10;
		else
			trackLength--;
	}
	
	if(trackLength < TRACKLENGTH_MIN)
		trackLength = TRACKLENGTH_MIN;
	if(trackLength > TRACKLENGTH_MAX)
		trackLength = TRACKLENGTH_MAX;
	itoa(trackLength, lcdbuff, 10);
	lcdmenu1_writebuff(lcdbuff);
	lcd_gotoxy(lcdcursor_POSEDITINIT,1);
}

void menuitem1sub3_exit(void)
{
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) 
	{
		menuitem_eevar.trackLength = trackLength;
		menuitem_eepromwrite();
	}
}


//Pitch (mm). This is used to convert rotational distance to linear distance
unsigned int pitch = 0;
#define PITCH_MAX 65535
#define PITCH_MIN 1
void menuitem1sub4_enter(void)
{
	//Save variable to eeprom if menu editing is done
	if(!lcdmenu1_isediting()) 
	{
		menuitem_eepromread();
		pitch = menuitem_eevar.pitch;
	}
	
	//Pressing up button to increase value
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			pitch += 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			pitch += 10;
		else
			pitch++;
	} 
	//Pressing down button will decrease value
	else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			pitch -= 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			pitch -= 10;
		else
			pitch--;
	}
	
	if(pitch < PITCH_MIN)
		pitch = PITCH_MIN;
	if(pitch > PITCH_MAX)
		pitch = PITCH_MAX;
	itoa(pitch, lcdbuff, 10);
	lcdmenu1_writebuff(lcdbuff);
	lcd_gotoxy(lcdcursor_POSEDITINIT,1);
}

void menuitem1sub4_exit(void)
{
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) 
	{
		menuitem_eevar.pitch = pitch;
		menuitem_eepromwrite();
	}
}


//Number of teeth. This is used to convert rotational distance to linear distance
unsigned char teeth = 0;
#define TEETH_MAX 255  
#define TEETH_MIN 1
void menuitem1sub5_enter(void)
{
	//Save variable to eeprom if menu editing is done
	if(!lcdmenu1_isediting()) 
	{
		menuitem_eepromread();
		teeth = menuitem_eevar.teeth;
	}
	
	//Pressing up button to increase value
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			teeth += 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			teeth += 10;
		else
			teeth++;
	} 
	//Pressing down button will decrease value
	else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			teeth -= 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			teeth -= 10;
		else
			teeth--;
	}
	
	if(teeth < TEETH_MIN)
		teeth = TEETH_MIN;
	if(teeth > TEETH_MAX)
		teeth = TEETH_MAX;
	itoa(teeth, lcdbuff, 10);
	lcdmenu1_writebuff(lcdbuff);
	lcd_gotoxy(lcdcursor_POSEDITINIT,1);
}

void menuitem1sub5_exit(void)
{
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) 
	{
		menuitem_eevar.teeth = teeth;
		menuitem_eepromwrite();
	}
}

//----------Menu 2: Camera Settings---------------

//Shutter Speed in seconds
//TODO: figure out how to do fractional seconds without loosing resolution
unsigned char shutterSpeed = 0;
#define SHUTTERSPEED_MAX 255  
#define SHUTTERSPEED_MIN 1
void menuitem2sub1_enter(void)
{
	//Save variable to eeprom if menu editing is done
	if(!lcdmenu1_isediting()) 
	{
		menuitem_eepromread();
		shutterSpeed = menuitem_eevar.shutterSpeed;
	}
	
	//Pressing up button to increase value
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			shutterSpeed += 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			shutterSpeed += 10;
		else
			shutterSpeed++;
	} 
	//Pressing down button will decrease value
	else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			shutterSpeed -= 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			shutterSpeed -= 10;
		else
			shutterSpeed--;
	}
	
	if(shutterSpeed < SHUTTERSPEED_MIN)
		shutterSpeed = SHUTTERSPEED_MIN;
	if(shutterSpeed > SHUTTERSPEED_MAX)
		shutterSpeed = SHUTTERSPEED_MAX;
	itoa(shutterSpeed, lcdbuff, 10);
	lcdmenu1_writebuff(lcdbuff);
	lcd_gotoxy(lcdcursor_POSEDITINIT,1);
}

void menuitem2sub1_exit(void)
{
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) {
		menuitem_eevar.shutterSpeed = shutterSpeed;
		menuitem_eepromwrite();
	}
}


//Pic Delay in seconds. This is time to wait after taking a picture. It is basically used so camera 
//  can finish processing. Usually it is 1 seconds per 15 seconds exposure. But can be higher
//  or lower based on the card read/write speed and camera model. 
unsigned char picDelay = 0;
#define PICDELAY_MAX 255  
#define PICDELAY_MIN 0
void menuitem2sub2_enter(void)
{
	//Save variable to eeprom if menu editing is done
	if(!lcdmenu1_isediting()) 
	{
		menuitem_eepromread();
		picDelay = menuitem_eevar.picDelay;
	}
	
	//Pressing up button to increase value
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			picDelay += 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			picDelay += 10;
		else
			picDelay++;
	} 
	//Pressing down button will decrease value
	else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			picDelay -= 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			picDelay -= 10;
		else
			picDelay--;
	}
	
	if(picDelay < PICDELAY_MIN)
		picDelay = PICDELAY_MIN;
	if(picDelay > PICDELAY_MAX)
		picDelay = PICDELAY_MAX;
	itoa(picDelay, lcdbuff, 10);
	lcdmenu1_writebuff(lcdbuff);
	lcd_gotoxy(lcdcursor_POSEDITINIT,1);
}

void menuitem2sub2_exit(void)
{
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) 
	{
		menuitem_eevar.picDelay = picDelay;
		menuitem_eepromwrite();
	}
}


//Motor Delay in seconds. This is used to add delay before motor starts moving. This is used to 
//  make sure motor doesn't move before picture is finished taking. 
unsigned char motorDelay = 0;
#define PICDELAY_MAX 255  
#define PICDELAY_MIN 0
void menuitem2sub3_enter(void)
{
	//Save variable to eeprom if menu editing is done
	if(!lcdmenu1_isediting()) 
	{
		menuitem_eepromread();
		motorDelay = menuitem_eevar.motorDelay;
	}
	
	//Pressing up button to increase value
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			motorDelay += 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			motorDelay += 10;
		else
			motorDelay++;
	} 
	//Pressing down button will decrease value
	else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			motorDelay -= 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			motorDelay -= 10;
		else
			motorDelay--;
	}
	
	if(motorDelay < PICDELAY_MIN)
		motorDelay = PICDELAY_MIN;
	if(motorDelay > PICDELAY_MAX)
		motorDelay = PICDELAY_MAX;
	itoa(motorDelay, lcdbuff, 10);
	lcdmenu1_writebuff(lcdbuff);
	lcd_gotoxy(lcdcursor_POSEDITINIT,1);	
}

void menuitem2sub3_exit(void)
{
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) 
	{
		menuitem_eevar.motorDelay = motorDelay;
		menuitem_eepromwrite();
	}
}

//TIMELAPSE PERIOD which is total length of whole time lapse.
unsigned int timelapsePeriod = 0;
#define TIMELAPSEPERIOD_MAX 65535  
#define TIMELAPSEPERIOD_MIN 0
void menuitem2sub4_enter(void)
{
	//Save variable to eeprom if menu editing is done
	if(!lcdmenu1_isediting()) 
	{
		menuitem_eepromread();
		timelapsePeriod = menuitem_eevar.timelapsePeriod;
	}
	
	//Pressing up button to increase value
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_UP) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			timelapsePeriod += 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			timelapsePeriod += 10;
		else
			timelapsePeriod++;
	} 
	//Pressing down button will decrease value
	else if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_DOWN) 
	{
		if(button_presscount > BUTTON_PRESSCOUNTMAX100)
			timelapsePeriod -= 100;
		else if(button_presscount > BUTTON_PRESSCOUNTMAX10)
			timelapsePeriod -= 10;
		else
			timelapsePeriod--;
	}
	
	if(timelapsePeriod < TIMELAPSEPERIOD_MIN)
		timelapsePeriod = TIMELAPSEPERIOD_MIN;
	if(timelapsePeriod > TIMELAPSEPERIOD_MAX)
		timelapsePeriod = TIMELAPSEPERIOD_MAX;
	itoa(timelapsePeriod, lcdbuff, 10);
	lcdmenu1_writebuff(lcdbuff);
	lcd_gotoxy(lcdcursor_POSEDITINIT,1);	
}

void menuitem2sub4_exit(void)
{
	if(lcdmenu1_buttonpressed == LCDMENU1_BUTTONPRESSED_RIGHT) 
	{
		menuitem_eevar.timelapsePeriod = timelapsePeriod;
		menuitem_eepromwrite();
	}
}


//----------Menu 3: Initialize---------------
//Initialize Right 
//TODO: This function will the system to right end. May need to do this in some other function...
void menuitem3sub1_enter(void)
{
	//Check to see if youre editing or pressed enter to go into the menu before initializing..
	if(lcdmenu1_isediting()) 
	{
		init_right = 1;
	}
	
}

void menuitem3sub1_exit(void)
{
	init_right = 0;
}

//Initialize Left 
//TODO: This function will the system to left end. May need to do this in some other function...
void menuitem3sub2_enter(void)
{
	//Check to see if youre editing or pressed enter to go into the menu before initializing..
	if(lcdmenu1_isediting()) 
	{
		init_left = 1;
	}
	
}

void menuitem3sub2_exit(void)
{
	init_left = 0;
}

//Start TimeLapse
void menuitem4_enter(void)
{
	if(lcdmenu1_isediting()) 
	{
		startTimelapse = 1;
	}
}

void menuitem4_exit(void)
{
	startTimelapse = 0;
}



//-------------------------------------------------------------------------------------
void menuitem_select(void) 
{
	micromenu_itemT* curritem = lcdmenu1_curritem();
	lcd_puts_p((const char*)(curritem->text));
	lcd_gotoxy(lcdcursor_POSEDITINIT,1);
}


/*
 * built menu
 * 
 * lcdmenu1_makemenu(name, next, prev, parent, sibling, selectfunc, enterfunc, exitfunc, text);
 * @param name is the name of the menuitem
 * @param next is the pointer to next menuitem
 * @param prev is the pointer to the prev menuitem
 * @param parent is the parent of the menuitem
 * @param sibling is the child of the menuitem
 * @param enterfunc is the function to be called when user enter this menuitem
 * @param exitfunc is the function to be called when user exit this menuitem
 * @param text is the text view on lcd for the menuitem

 */
 

// lcdmenu1_makemenu(menuitem1, menuitem2, menuitem4, MICROMENU_NULLENTRY, menuitem1sub1, menuitem_select, MICROMENU_NULLFUNC, MICROMENU_NULLFUNC, "menu1"); //sample category
// lcdmenu1_makemenu(menuitem2, menuitem3, menuitem1, MICROMENU_NULLENTRY, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem2_enter, menuitem2_exit, "item (int)"); //sample item
// lcdmenu1_makemenu(menuitem3, menuitem4, menuitem2, MICROMENU_NULLENTRY, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem3_enter, menuitem3_exit, "item (bool)"); //sample item
// lcdmenu1_makemenu(menuitem4, menuitem1, menuitem3, MICROMENU_NULLENTRY, MICROMENU_NULLENTRY, menuitem_select, MICROMENU_NULLFUNC, MICROMENU_NULLFUNC, "menu2"); //sample category
// lcdmenu1_makemenu(menuitem1sub1, menuitem1sub2, menuitem1sub2, menuitem1, menuitem1sub1sub1, menuitem_select, MICROMENU_NULLFUNC, MICROMENU_NULLFUNC, "menu1sub1"); //sample category
// lcdmenu1_makemenu(menuitem1sub2, menuitem1sub1, menuitem1sub1, menuitem1, MICROMENU_NULLENTRY, menuitem_select, MICROMENU_NULLFUNC, MICROMENU_NULLFUNC, "menu1sub2"); //sample category
// lcdmenu1_makemenu(menuitem1sub1sub1, menuitem1sub1sub1, menuitem1sub1sub1, menuitem1sub1, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, MICROMENU_NULLFUNC, MICROMENU_NULLFUNC, "menu1sub1sub1"); //sample category

//timescape menu builder.
//Main menu items with submenu
lcdmenu1_makemenu(menuitem1, menuitem2, menuitem4, MICROMENU_NULLENTRY, menuitem1sub1, menuitem_select, MICROMENU_NULLFUNC, MICROMENU_NULLFUNC, "Preferences");		// Preference menu
lcdmenu1_makemenu(menuitem2, menuitem3, menuitem1, MICROMENU_NULLENTRY, menuitem2sub1, menuitem_select, MICROMENU_NULLFUNC, MICROMENU_NULLFUNC, "Camera Settings"); // Camera Settings Menu
lcdmenu1_makemenu(menuitem3, menuitem4, menuitem2, MICROMENU_NULLENTRY, menuitem3sub1, menuitem_select, MICROMENU_NULLFUNC, MICROMENU_NULLFUNC, "Initialize"); 		//Initialize

//Main menu item with no submenu
lcdmenu1_makemenu(menuitem4, menuitem1, menuitem3, MICROMENU_NULLENTRY, MICROMENU_NULLENTRY, menuitem_select, menuitem4_enter, menuitem4_exit, "Start TL"); //Start TimeLapse

//Preferences SubMenu
// lcdmenu1_makemenu(menuitem1sub2, menuitem1sub1, menuitem1sub1, menuitem1, MICROMENU_NULLENTRY, menuitem_select, MICROMENU_NULLFUNC, MICROMENU_NULLFUNC, "menu1sub2"); //sample category
// lcdmenu1_makemenu(menuitem2, menuitem3, menuitem1, MICROMENU_NULLENTRY, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem2_enter, menuitem2_exit, "item (int)"); //sample item
lcdmenu1_makemenu(menuitem1sub1, menuitem1sub2, menuitem1sub5, menuitem1, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem1sub1_enter, menuitem1sub1_exit, "Motor RPM");		// Preference submenu
lcdmenu1_makemenu(menuitem1sub2, menuitem1sub3, menuitem1sub1, menuitem1, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem1sub2_enter, menuitem1sub2_exit, "Mot. Steps/Rev");	// Preference submenu
lcdmenu1_makemenu(menuitem1sub3, menuitem1sub4, menuitem1sub2, menuitem1, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem1sub3_enter, menuitem1sub3_exit, "Track (mm)");		// Preference submenu
lcdmenu1_makemenu(menuitem1sub4, menuitem1sub5, menuitem1sub3, menuitem1, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem1sub4_enter, menuitem1sub4_exit, "Pitch (um)");		// Preference submenu
lcdmenu1_makemenu(menuitem1sub5, menuitem1sub1, menuitem1sub4, menuitem1, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem1sub5_enter, menuitem1sub5_exit, "Teeth");			// Preference submenu


//Camera Settings SubMenu
lcdmenu1_makemenu(menuitem2sub1, menuitem2sub2, menuitem2sub4, menuitem2, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem2sub1_enter, menuitem2sub1_exit, "Shutter (s)");		// Camera Settings submenu
lcdmenu1_makemenu(menuitem2sub2, menuitem2sub3, menuitem2sub1, menuitem2, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem2sub2_enter, menuitem2sub2_exit, "Pic Delay(s)");	// Camera Settings submenu
lcdmenu1_makemenu(menuitem2sub3, menuitem2sub4, menuitem2sub2, menuitem2, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem2sub3_enter, menuitem2sub3_exit, "Motor Delay(s)");	// Camera Settings submenu
lcdmenu1_makemenu(menuitem2sub4, menuitem2sub1, menuitem2sub3, menuitem2, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem2sub4_enter, menuitem2sub4_exit, "Timelapse(min)");	// Camera Settings submenu

//Initialize
lcdmenu1_makemenu(menuitem3sub1, menuitem3sub2, menuitem3sub2, menuitem3, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem3sub1_enter, menuitem3sub1_exit, "Init Right");	// Initialize submenu
lcdmenu1_makemenu(menuitem3sub2, menuitem3sub1, menuitem3sub1, menuitem3, MICROMENU_NULLENTRY, MICROMENU_NULLFUNC, menuitem3sub2_enter, menuitem3sub2_exit, "Init Left");	// Initialize submenu




void menuitem_initialize()
{
	//init eeprom
	menuitem_eepromread();
	if(((int)menuitem_eevar.initeeprom & 0XFF) == 0xFF) { //init values
		menuitem_eeprominit();
	}

	//init lcdmenu1
	lcdmenu1_init((micromenu_itemT*)&menuitem1);
}

//These functions can be called from outside the class.

unsigned char GetMotorRPM()
{
	return menuitem_eevar.motorRPM;
}

unsigned int GetStepsPerRev()
{
	return menuitem_eevar.stepsPerRev;
}

unsigned int GetTrackLength()
{
	return  menuitem_eevar.trackLength;
}

unsigned int GetPitch()
{
	return menuitem_eevar.pitch;
}

unsigned char GetTeeth()
{
	return menuitem_eevar.teeth;
}
unsigned char GetShutterSpeed()
{
	return menuitem_eevar.shutterSpeed;
}

unsigned char GetPicDelay()
{
	return menuitem_eevar.picDelay;
}

unsigned char GetMotorDelay()
{
	return menuitem_eevar.motorDelay;
}

unsigned int GetTimelapsePeriod()
{
	return menuitem_eevar.timelapsePeriod;
}

// following line turns on automatic (because I am lazy or smart, there is a fine line) indentation for Kate editor.
// kate: space-indent on; indent-width 5; mixedindent off; indent-mode cstyle;