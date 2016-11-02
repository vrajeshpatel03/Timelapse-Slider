/*
lcdmenu1 0x02

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LCDMENU1_H_
#define LCDMENU1_H_

//include external library functions
#include "lcd.h"
#include "micromenu.h"

//button
#define LCDMENU1_BUTTONPRESSED_NONE 0
#define LCDMENU1_BUTTONPRESSED_UP 1
#define LCDMENU1_BUTTONPRESSED_DOWN 2
#define LCDMENU1_BUTTONPRESSED_RIGHT 3
#define LCDMENU1_BUTTONPRESSED_LEFT 4
#define LCDMENU1_BUTTONPRESSED_SELECT 5

//pressed button
extern volatile uint8_t lcdmenu1_buttonpressed;

//set a menu
#define lcdmenu1_setmenu(x) \
		micromenu_menuchange((micromenu_itemT*)&x); \
		lcdmenu1_writemenu();
//run a menu function
#define lcdmenu1_runmenu(x) \
		lcdmenu1_writemenu(); \
		micromenu_menufunc((micromenu_funcT*)&x);
//define a menu
//name is the name of the menuitem
//next is the pointer to next menuitem
//prev is the pointer to the prev menuitem
//parent is the parent of the menuitem
//sibling is the child of the menuitem
//selectfunc is the function to be called when user press select on this menuitem
//enterfunc is the function to be called when user enter this menuitem
//exitfunc is the function to be called when user exit this menuitem
//text is the text view on lcd for the menuitem
#define lcdmenu1_makemenu(name, next, prev, parent, sibling, selectfunc, enterfunc, exitfunc, text) \
		micromenu_makemenu(name, next, prev, parent, sibling, selectfunc, enterfunc, exitfunc, text);

//functions
extern void lcdmenu1_init(micromenu_itemT *menuitem);
extern void lcdmenu1_RIGHT();
extern void lcdmenu1_LEFT();
extern void lcdmenu1_UP();
extern void lcdmenu1_DOWN();
extern void lcdmenu1_SELECT();
extern void lcdmenu1_writebuff(char *l);
extern uint8_t lcdmenu1_isediting();
extern micromenu_itemT* lcdmenu1_curritem();

#endif

#ifdef __cplusplus
}
#endif
