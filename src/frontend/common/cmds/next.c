/*
 * Command Name:	next.c
 * Description:		Next Window Command
 */

#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>

int fe_common_cmd_next(char *env, char *args)
{
	struct fe_widget *target, *widget;

	if (!(target = (struct fe_widget *) fe_get_target(NULL, "frame")))
		return(-1);
	if (fe_widget_control(target, WCC_NEXT_WIDGET, &widget) || !widget)
		fe_widget_control(target, WCC_FIRST_WIDGET, NULL);
	return(0);
}


