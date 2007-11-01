/*
 * Command Name:	scroll.c
 * Description:		Scroll Current Window Command
 */

#include <stdlib.h>

#include <stutter/utils.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_scroll(char *env, char *args)
{
	char *num;
	void *window;

	if (!(window = fe_get_target(NULL, "text")))
		return(-1);
	num = util_get_arg(args, NULL);
	fe_scroll(window, atoi(num));
	return(0);
}


