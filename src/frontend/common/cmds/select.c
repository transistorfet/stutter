/*
 * Command Name:	select.c
 * Description:		Select Window Command
 */

#include <stutter/utils.h>
#include <stutter/frontend/frontend.h>

int fe_common_cmd_select(char *env, char *args)
{
	char *id;
	struct fe_widget *parent, *child;

	id = util_get_arg(args, NULL);
	if (!(child = fe_find_widget(id)))
		return(-1);
	for (parent = child->parent; parent; child = parent, parent = parent->parent)
		fe_widget_control(parent, WCC_SHOW_WIDGET, child);
	return(0);
}


