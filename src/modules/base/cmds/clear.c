/*
 * Command Name:	clear.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Clear Current Window Command
 */

#include <stdlib.h>

#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_clear(char *env, char *args)
{
	void *window;

	if (!(window = fe_current_widget("text", NULL)) && !(window = fe_first_widget("text", NULL)))
		return(-1);
	fe_clear(window);
	return(0);
}


