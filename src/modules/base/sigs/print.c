/*
 * Handler Name:	print.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Send a string to the window stored with the handler or
 *			the current window if NULL is given as the window.
 */

#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_sig_print(void *window, void *index, char *str)
{
	if (!window && !(window = fe_current_widget("text", NULL)) && !(window = fe_first_widget("text", NULL)))
		return(-1);
	fe_print(window, str);
	return(0);
}


