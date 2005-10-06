/*
 * Command Name:	previous.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Previous Window Command
 */

#include <frontend.h>
#include <modules/base.h>

int base_cmd_previous(char *env, char *str)
{
	void *window;

	if (!(window = fe_previous_widget()) && !(window = fe_last_widget()))
		return(-1);
	return(0);
}


