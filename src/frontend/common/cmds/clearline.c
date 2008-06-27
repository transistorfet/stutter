/*
 * Command Name:	clearline.c
 * Description:		Clear Current Input Field Command
 */

#include <stdlib.h>

#include <stutter/frontend/frontend.h>

int fe_common_cmd_clearline(char *env, char *args)
{
	void *input;

	if (!(input = fe_get_focus("input")))
		return(-1);
	fe_clear(input);
	return(0);
}


