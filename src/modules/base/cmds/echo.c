/*
 * Command Name:	echo.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Echo Command
 */


#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_echo(char *env, char *args)
{
	void *window;

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	fe_print(window, args);
	return(0);
}


