/*
 * Command Name:	clearline.c
 * Description:		Clear Current Input Field Command
 */

#include <stutter/memory.h>
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>

int fe_common_cmd_clearline(char *env, char *args)
{
	struct fe_widget *widget;

	if (!(widget = fe_get_focus("input")))
		return(-1);
	// TODO this is dangerous passing a null surface...
	FE_WIDGET_CLEAR(widget, NULL);
	return(0);
}


