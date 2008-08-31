/*
 * Handler Name:	print.c
 * Description:		Print a text message to a stored window
 */

#include CONFIG_H
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>

int fe_common_sig_print(struct fe_widget *widget, const char *text)
{
	/** If widget is NULL, print to the current window */
	if (!widget && !(widget = fe_get_target(NULL, "text")))
		return(-1);
	FE_WIDGET_WRITE(widget, text, -1);
	return(0);
}


