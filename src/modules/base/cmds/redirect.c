/*
 * Command Name:	redirect.c
 * Version:		0.1
 * Module Requirements:	signal ; frontend ; modbase
 * Description:		Redirect Signal to Window Command
 */

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/modules/base.h>

int base_cmd_redirect(char *env, char *args)
{
	void *window;
	char *name, *target;
	char buffer[SMALL_STRING_SIZE];

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	trim_whitespace_m(args);
	get_param_m(args, name, ' ');
	get_param_m(args, target, ' ');

	if (*args != '\0') {
		BASE_ERROR_JOINPOINT("redirect: Invalid parameters\n")
		return(-1);
	}
	else if (!strcmp(target, "none")) {
		signal_disconnect(name, NULL, (signal_t) base_sig_print, NULL);
		if (snprintf(buffer, SMALL_STRING_SIZE, "Signal %s has been disconnected", name) >= 0)
			fe_print(window, buffer);
	}
	else if (!strcmp(target, "current")) {
		signal_connect(name, NULL, 10, (signal_t) base_sig_print, NULL);
		if (snprintf(buffer, SMALL_STRING_SIZE, "Signal %s is directed to the current window", name) >= 0)
			fe_print(window, buffer);
	}
	else if (!strcmp(target, "this")) {
		signal_connect(name, NULL, 10, (signal_t) base_sig_print, window);
		if (snprintf(buffer, SMALL_STRING_SIZE, "Signal %s is directed to this window", name) >= 0)
			fe_print(window, buffer);
	}
	else {
		BASE_ERROR_JOINPOINT("redirect: Invalid target location\n")
		return(-1);
	}
	return(0);
}


