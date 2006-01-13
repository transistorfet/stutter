/*
 * Command Name:	clear.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Clear Current Window Command
 */

#include <stdlib.h>

#include <frontend.h>
#include <lib/macros.h>
#include <modules/base.h>

int base_cmd_clear(char *env, char *args)
{
	void *window;

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	fe_clear(window);
	return(0);
}


