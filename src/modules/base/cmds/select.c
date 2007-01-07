/*
 * Command Name:	select.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Select Window Command
 */

#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_select(char *env, char *args)
{
	char *id;
	void *window;

	get_param_m(args, id, ' ');
	if (!(window = fe_find_widget(id)))
		return(-1);
	if (!fe_select_widget("text", NULL, window))
		return(-1);
	return(0);
}


