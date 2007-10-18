/*
 * Handler Name:	print.c
 * Description:		Send a string to the window stored with the handler or
 *			the current window if NULL is given as the window.
 */

#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_sig_print(void *window, void *index, char *str)
{
	if (!window && !(window = fe_get_target(NULL, "text")))
		return(-1);
	fe_print(window, str);
	return(0);
}


