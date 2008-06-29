/*
 * Command Name:	scroll.c
 * Description:		Scroll Current Window Command
 */

#include <stutter/utils.h>
#include <stutter/memory.h>
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>

int fe_common_cmd_scroll(char *env, char *args)
{
	char *num;
	struct fe_widget *widget;

	if (!(widget = fe_get_target(NULL, "text")))
		return(-1);
	num = util_get_arg(args, NULL);
	fe_widget_control(widget, WCC_SCROLL, atoi(num));
	return(0);
}


