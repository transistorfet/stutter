/*
 * Command Name:	echo.c
 * Description:		Echo Command
 */


#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_echo(char *env, char *args)
{
	void *window;

	if (!(window = fe_get_target(NULL, "text")))
		return(-1);
	fe_print(window, args);
	return(0);
}


