/*
 * Command Name:	scroll.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Scroll Current Window Command
 */

#include <stdlib.h>

#include <frontend.h>
#include <lib/macros.h>
#include <modules/base.h>

int base_cmd_scroll(char *env, char *args)
{
	char *num;
	void *window;

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	get_param_m(args, num, ' ');
	fe_scroll(window, atoi(num));
	return(0);
}


