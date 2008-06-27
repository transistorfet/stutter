/*
 * Command Name:	previous.c
 * Description:		Previous Window Command
 */

#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>

int fe_common_cmd_previous(char *env, char *args)
{
	struct fe_widget *target, *widget;

	if (!(target = (struct fe_widget *) fe_get_target(NULL, "frame")))
		return(-1);
	if (fe_widget_control(target, WCC_PREVIOUS_WIDGET, &widget) || !widget)
		fe_widget_control(target, WCC_LAST_WIDGET, NULL);
	return(0);
}


