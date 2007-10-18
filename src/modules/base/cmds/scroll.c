/*
 * Command Name:	scroll.c
 * Description:		Scroll Current Window Command
 */

#include <stdlib.h>

#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_scroll(char *env, char *args)
{
	char *num;
	void *window;

	if (!(window = fe_get_target(NULL, "text")))
		return(-1);
	get_param_m(args, num, ' ');
	fe_scroll(window, atoi(num));
	return(0);
}


