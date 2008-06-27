/*
 * Command Name:	select.c
 * Description:		Select Window Command
 */

#include <stutter/utils.h>
#include <stutter/frontend/frontend.h>

int fe_common_cmd_select(char *env, char *args)
{
	char *id;
	void *window;

	id = util_get_arg(args, NULL);
	if (!(window = fe_find_widget(id)))
		return(-1);
	if (!fe_show_widget(window))
		return(-1);
	return(0);
}


