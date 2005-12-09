/*
 * Command Name:	previous.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Previous Window Command
 */

#include <frontend.h>
#include <modules/base.h>

int base_cmd_previous(char *env, char *args)
{
	void *window;

	if (!(window = fe_previous_widget("window", NULL)) && !(window = fe_last_widget("window", NULL)))
		return(-1);
	return(0);
}


