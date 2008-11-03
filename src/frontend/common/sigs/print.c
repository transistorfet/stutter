/*
 * Handler Name:	print.c
 * Description:		Print a text message to a stored window
 */

#include CONFIG_H
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>

int fe_common_sig_print(struct fe_widget *widget, struct signal_s *signal, va_list va)
{
	const char *text;

	text = va_arg(va, const char *);
	// TODO check to make sure the signal is actually a print
	/** If widget is NULL, print to the current window */
	if (!widget && !(widget = fe_get_target(NULL, "text")))
		return(-1);
	FE_WIDGET_WRITE(widget, text, -1);
	return(0);
}


