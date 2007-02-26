/*
 * Command Name:	redirect.c
 * Version:		0.1
 * Module Requirements:	signal ; frontend ; modbase
 * Description:		Redirect Signal to Window Command
 */

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_redirect(char *env, char *args)
{
	void *window;
	char *name, *target;

	trim_whitespace_m(args);
	get_param_m(args, name, ' ');
	get_param_m(args, target, ' ');

	if (*args != '\0')
		return(-1);
	else if (!strcmp(target, "none")) {
		signal_disconnect(signal_find_handler(NULL, name, (signal_t) base_sig_print, NULL));
		BASE_COMMAND_RESPONSE_JOINPOINT(BASE_FMT_REDIRECT_DISCONNECTED, name)
	}
	else if (!strcmp(target, "current")) {
		signal_connect(NULL, name, 10, (signal_t) base_sig_print, NULL);
		BASE_COMMAND_RESPONSE_JOINPOINT(BASE_FMT_REDIRECT_CURRENT, name)
	}
	else if (!strcmp(target, "this")) {
		signal_connect(NULL, name, 10, (signal_t) base_sig_print, window);
		BASE_COMMAND_RESPONSE_JOINPOINT(BASE_FMT_REDIRECT_THIS, name)
	}
	else {
		BASE_ERROR_JOINPOINT(BASE_ERR_REDIRECT_FAILED, target)
		return(-1);
	}
	return(0);
}


