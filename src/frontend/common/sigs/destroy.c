/*
 * Handler Name:	destroy.c
 * Description:		Handle a destroy signal
 */

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/frontend/common/widget.h>

int fe_common_sig_destroy(struct fe_widget *widget, struct signal_s *signal, va_list va)
{
	// TODO destroy widget
	// TODO destroy handler
	return(0);
}


