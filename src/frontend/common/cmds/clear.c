/*
 * Command Name:	clear.c
 * Description:		Clear Current Window Command
 */

#include <stdlib.h>

#include <stutter/frontend/frontend.h>

int fe_common_cmd_clear(char *env, char *args)
{
	void *window;

	if (!(window = fe_get_target(NULL, "text")))
		return(-1);
	fe_clear(window);
	return(0);
}


