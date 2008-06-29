/*
 * Command Name:	close.c
 * Description:		Close Window Command
 */

#include <stutter/utils.h>
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>

int fe_common_cmd_close(char *env, char *args)
{
	char *name;
	struct fe_widget *widget;

	name = util_get_arg(args, NULL);
	if (*name != '\0') {
		if (!(widget = fe_find_widget(name)))
			return(-1);
	}
	else if (!(widget = fe_get_target(NULL, "text")))
		return(-1);
	if (FE_WIDGET(widget)->parent) {
		fe_widget_control(FE_WIDGET(widget)->parent, WCC_REMOVE_WIDGET, widget);
		destroy_object(OBJECT_S(widget));
	}
	return(0);
}


