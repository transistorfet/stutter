/*
 * Command Name:	clearline.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Clear Current Input Field Command
 */

#include <stdlib.h>

#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/modules/base.h>

int base_cmd_clearline(char *env, char *args)
{
	void *input;

	if (!(input = fe_current_widget("input", NULL)))
		return(-1);
	fe_clear(input);
	return(0);
}


