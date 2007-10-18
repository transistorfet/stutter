/*
 * Module Name:		init.c
 * Description:		IRC Module Initializer
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/modules/irc.h>
#include <stutter/modules/base.h>

int init_irc(void)
{
	add_signal(NULL, "irc.error", 0);
	init_irc_server();

	IRC_INIT_JOINPOINT()

	return(0);
}

int release_irc(void)
{
	struct type_s *type;

	IRC_RELEASE_JOINPOINT()

	/** Remove the whole variable table */
	if ((type = find_type("table"))) {
		add_variable(NULL, type, "irc", 0, "");
		remove_variable(NULL, type, "irc");
	}

	release_irc_server();
	remove_signal(NULL, "irc.error");
	return(0);
}


