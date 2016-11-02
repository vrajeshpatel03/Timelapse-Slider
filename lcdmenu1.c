/*lcdmenu1 0x02

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include <stdlib.h>
#include <string.h>

#include "lcdmenu1.h"


volatile uint8_t lcdmenu1_editing = 0;
volatile uint8_t lcdmenu1_buttonpressed;
volatile char lcdbuff[16];


/*
 * write menu to lcd
 */
void lcdmenu1_writemenu() {
	//clrlcd
	lcd_clrscr();
	//write the menu name
	lcd_gotoxy(1,0);
	lcd_puts_p((const char*)(micromenu_curritem->text));
	lcd_gotoxy(0,0);
	//check if it has childs
	if((void*)&MICROMENU_SIBLING != (void*)&MICROMENU_NULLENTRY) {
		//this is an closed menu
		lcd_puts_p(PSTR("+"));
	} else {
		//check if it has function
		if(&MICROMENU_ENTERFUNC != MICROMENU_NULLFUNC) {
			if(!lcdmenu1_editing) {
				//this item is not in editing mode
				lcd_puts_p(PSTR(">"));
				lcd_gotoxy(0,1);
				micromenu_menufunc((micromenu_funcT*)&MICROMENU_ENTERFUNC);
			} else {
				//this item is in editing mode
				lcd_puts_p(PSTR("<"));
			}
		} else {
			//this is an open menu
			lcd_puts_p(PSTR("-"));
		}
	}
	//check if it has parent
	if((void*)&MICROMENU_PARENT != (void*)&MICROMENU_NULLENTRY) {
		lcd_gotoxy(15,0);
		lcd_puts_p(PSTR("<"));
	}
	lcd_gotoxy(0,1);
}


/*
 * init lcdmenu
 */
void lcdmenu1_init(micromenu_itemT *menuitem) {
	lcdmenu1_buttonpressed = LCDMENU1_BUTTONPRESSED_NONE;
	lcdmenu1_setmenu(*menuitem);
}


/*
 * button pressed
 */
void lcdmenu1_RIGHT() {
	if(!lcdmenu1_editing) {
		if((void*)&MICROMENU_SIBLING != (void*)&MICROMENU_NULLENTRY) {
			lcdmenu1_setmenu(MICROMENU_SIBLING);
		} else if(&MICROMENU_ENTERFUNC != MICROMENU_NULLFUNC) {
			lcdmenu1_editing = 1;
			lcdmenu1_runmenu(MICROMENU_ENTERFUNC);
		}
	} else {
		lcdmenu1_buttonpressed = LCDMENU1_BUTTONPRESSED_RIGHT;
		lcdmenu1_runmenu(MICROMENU_EXITFUNC);
		lcdmenu1_buttonpressed = LCDMENU1_BUTTONPRESSED_NONE;
		lcdmenu1_editing = 0;
		lcdmenu1_writemenu();
	}
}


/*
 * button pressed
 */
void lcdmenu1_LEFT() {
	if(!lcdmenu1_editing) {
		if((void*)&MICROMENU_PARENT != (void*)&MICROMENU_NULLENTRY) {
			lcdmenu1_setmenu(MICROMENU_PARENT);
		}
	} else {
		lcdmenu1_buttonpressed = LCDMENU1_BUTTONPRESSED_LEFT;
		lcdmenu1_runmenu(MICROMENU_EXITFUNC);
		lcdmenu1_buttonpressed = LCDMENU1_BUTTONPRESSED_NONE;
		lcdmenu1_editing = 0;
		lcdmenu1_writemenu();
	}
}


/*
 * button pressed
 */
void lcdmenu1_UP() {
	if(!lcdmenu1_editing) {
		lcdmenu1_setmenu(MICROMENU_PREV);
	} else {
		lcdmenu1_buttonpressed = LCDMENU1_BUTTONPRESSED_UP;
		lcdmenu1_runmenu(MICROMENU_ENTERFUNC);
	}
}


/*
 * button pressed
 */
void lcdmenu1_DOWN() {
	if(!lcdmenu1_editing) {
		lcdmenu1_setmenu(MICROMENU_NEXT);
	} else {
		lcdmenu1_buttonpressed = LCDMENU1_BUTTONPRESSED_DOWN;
		lcdmenu1_runmenu(MICROMENU_ENTERFUNC);
	}
}

/*
 * button pressed
 */
void lcdmenu1_SELECT() {
	if(!lcdmenu1_editing) {
		lcdmenu1_buttonpressed = LCDMENU1_BUTTONPRESSED_SELECT;
		lcdmenu1_runmenu(MICROMENU_SELECTFUNC);
	}
}

/*
 * write to lcd the current menu item plus thest
 */
void lcdmenu1_writebuff(char *l) {
	lcd_puts(l);
}


/*
 * check if is editing status
 */
uint8_t lcdmenu1_isediting() {
	return lcdmenu1_editing;
}

/*
 * return the current item menu
 */
micromenu_itemT* lcdmenu1_curritem() {
	return micromenu_curritem;
}


