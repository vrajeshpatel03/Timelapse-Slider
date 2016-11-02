/*
micromenu 0x01

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include "micromenu.h"

micromenu_itemT micromenu_nullitem = {(void*)0, (void*)0, (void*)0, (void*)0, (void*)0, (void*)0, (void*)0, {0x00}};
micromenu_itemT* micromenu_curritem;


/*
 * go to selected menu
 */
void micromenu_menuchange(micromenu_itemT* menuitem) {
	if ((void*)menuitem == (void*)&MICROMENU_NULLENTRY)
		return;

	//set current item
	micromenu_curritem = menuitem;
	//run item function
	micromenu_runmenu(MICROMENU_ENTERFUNC);
}

/*
 * eval menu function
 */
void micromenu_menufunc(micromenu_funcT* menufunc) {
	if ((void*)menufunc == (void*)MICROMENU_NULLFUNC)
		return;

	((micromenu_funcT)menufunc)();
}


