/*
 * Command Name:	previous.c
 * Description:		Previous Window Command
 */

#include <stutter/frontend.h>
#include <stutter/frontend/widget.h>

int fe_common_cmd_previous(char *env, char *args)
{
	struct widget_s *target, *widget;

	if (!(target = (struct widget_s *) fe_get_target(NULL, "frame")))
		return(-1);
	if (widget_control(target, WCC_PREVIOUS_WIDGET, &widget) || !widget)
		widget_control(target, WCC_LAST_WIDGET, NULL);
	return(0);
}


