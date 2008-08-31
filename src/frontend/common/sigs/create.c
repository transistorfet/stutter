/*
 * Handler Name:	create.c
 * Description:		Create a new window on a create signal
 */

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/common.h>

int fe_common_sig_create(void *ptr, const char *name)
{
	struct fe_widget *widget;

	if (!name || (*name == '\0'))
		return(-1);
	if (!(widget = fe_find_widget(name)) || !(widget = fe_create_widget("text", name)))
		// TODO should you print an error message here?
		return(-1);
	signal_named_connect(signal_table, name, 10, (signal_t) fe_common_sig_print, widget);
	return(0);
}


