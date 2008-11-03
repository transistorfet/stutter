/*
 * Handler Name:	create.c
 * Description:		Create a new window on a create signal
 */

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/common.h>

int fe_common_sig_create(void *ptr, struct signal_s *signal, va_list va)
{
	const char *name;
	struct fe_widget *widget;

	name = va_arg(va, const char *);
	// TODO check to make sure signal is a create signal and not another signal
	if (!name || (*name == '\0'))
		return(-1);
	if (!(widget = fe_find_widget(name)) || !(widget = fe_create_widget("text", name)))
		// TODO should you print an error message here?
		return(-1);
	fe_show_widget(widget);
	signal_named_connect(VARIABLE_S(signal_table), name, 10, (signal_func_t) fe_common_sig_print, widget);
	return(0);
}


