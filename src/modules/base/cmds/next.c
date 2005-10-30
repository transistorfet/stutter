/*
 * Command Name:	next.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Next Window Command
 */

#include <frontend.h>
#include <modules/base.h>

int base_cmd_next(char *env, char *args)
{
	void *window;

	if (!(window = fe_next_widget()) && !(window = fe_first_widget()))
		return(-1);
	return(0);
}


