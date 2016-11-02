/*
micromenu 0x01

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.

References:
  + micro-menu by Dean Camera, 2007
    www.fourwalledcubicle.com
*/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MICROMENU_H

#define MICROMENU_H

#include <avr/pgmspace.h>

//type menu function
typedef void (*micromenu_funcT)(void);
//type menu item
typedef struct {
	void *next;
	void *prev;
	void *parent;
	void *sibling;
	micromenu_funcT selectfunc;
	micromenu_funcT enterfunc;
	micromenu_funcT exitfunc;
	const char text[16];
} PROGMEM micromenu_itemT;

//null menu item
extern micromenu_itemT micromenu_nullitem;
//current menu item
extern micromenu_itemT* micromenu_curritem;

//default definitions
#define MICROMENU_NULLENTRY micromenu_nullitem
#define MICROMENU_NULLFUNC (void*)0
#define MICROMENU_NULLTEXT 0x00

//items browsing
#define MICROMENU_PREV *((micromenu_itemT*)pgm_read_word(&micromenu_curritem->prev))
#define MICROMENU_NEXT *((micromenu_itemT*)pgm_read_word(&micromenu_curritem->next))
#define MICROMENU_PARENT *((micromenu_itemT*)pgm_read_word(&micromenu_curritem->parent))
#define MICROMENU_SIBLING *((micromenu_itemT*)pgm_read_word(&micromenu_curritem->sibling))
#define MICROMENU_SELECTFUNC *((micromenu_funcT*)pgm_read_word(&micromenu_curritem->selectfunc))
#define MICROMENU_ENTERFUNC *((micromenu_funcT*)pgm_read_word(&micromenu_curritem->enterfunc))
#define MICROMENU_EXITFUNC *((micromenu_funcT*)pgm_read_word(&micromenu_curritem->exitfunc))

//functions definitions
#define micromenu_makemenu(name, next, prev, parent, sibling, selectfunc, enterfunc, exitfunc, text) \
		extern micromenu_itemT next; \
		extern micromenu_itemT prev; \
		extern micromenu_itemT parent; \
		extern micromenu_itemT sibling; \
		micromenu_itemT name = {(void*)&next, (void*)&prev, (void*)&parent, (void*)&sibling, (micromenu_funcT)selectfunc, (micromenu_funcT)enterfunc, (micromenu_funcT)exitfunc, { text }}
#define micromenu_setmenu(x) \
		micromenu_menuchange((micromenu_itemT*)&x);
#define micromenu_runmenu(x) \
		micromenu_menufunc((micromenu_funcT*)&x);
#define micromenu_externmenu(name) \
		extern micromenu_itemT name;

//functions
extern void micromenu_menuchange(micromenu_itemT* menuitem);
extern void micromenu_menufunc(micromenu_funcT* menufunc);

#endif

#ifdef __cplusplus
}
#endif