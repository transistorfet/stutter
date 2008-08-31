/*
 * Handler Name:	destroy.c
 * Description:		Handle a destroy signal
 */

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/frontend/common/widget.h>

int fe_common_sig_destroy(struct fe_widget *widget, const char *name)
{
	// TODO destroy widget
	// TODO destroy handler
	return(0);
}


