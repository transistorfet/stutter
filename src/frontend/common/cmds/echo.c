/*
 * Command Name:	echo.c
 * Description:		Echo Command
 */

#include <stutter/output.h>
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>

int fe_common_cmd_echo(char *env, char *args)
{
	struct fe_widget *widget;

	if (!(widget = fe_get_target(NULL, "text")))
		return(-1);
	FE_WIDGET_WRITE(widget, args, -1);
	return(0);
}


