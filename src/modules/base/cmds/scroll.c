/*
 * Command Name:	scroll.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Scroll Current Window Command
 */

#include <stdlib.h>

#include <macros.h>
#include <frontend.h>
#include <modules/base.h>

int base_cmd_scroll(char *env, char *str)
{
	char *num;
	void *window;

	if (!(window = fe_current_widget()) && !(window = fe_first_widget()))
		return(-1);
	get_param_m(str, num, ' ');
	fe_scroll(window, atoi(num));
	return(0);
}


