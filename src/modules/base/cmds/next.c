/*
 * Command Name:	next.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Next Window Command
 */

#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_next(char *env, char *args)
{
	void *window;

	if (!(window = fe_next_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	return(0);
}


