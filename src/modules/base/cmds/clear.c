/*
 * Command Name:	clear.c
 * Description:		Clear Current Window Command
 */

#include <stdlib.h>

#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_clear(char *env, char *args)
{
	void *window;

	if (!(window = fe_get_target(NULL, "text")))
		return(-1);
	fe_clear(window);
	return(0);
}


