/*
 * Command Name:	echo.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Echo Command
 */


#include <frontend.h>
#include <modules/base.h>

int base_cmd_echo(char *env, char *args)
{
	void *window;

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	fe_print(window, create_string("%s", args));
	return(0);
}


