/*
 * Module Name:		init.c
 * Version:		0.1
 * Module Requirements:	type ; signal ; string ; variable ; modirc
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

DEFINE_HANDLER_LIST(irc_handlers,
	IRC_HANDLERS()
);

DEFINE_VARIABLE_LIST(irc_variables,
	DECLARE_TYPE("string",
		ADD_FIXED_VARIABLE("version", "string", IRC_VERSION_RESPONSE)
	)
	DECLARE_TYPE("status",
		ADD_FIXED_VARIABLE("current_nick", "stringify,null", irc_stringify_nick, NULL)
		ADD_FIXED_VARIABLE("current_channel", "stringify,null", irc_stringify_channel, NULL)
	)
	DECLARE_TYPE("command",
		IRC_COMMANDS()
	)
);

DEFINE_KEY_LIST(irc_keys,
	IRC_BINDINGS()
);

static void *irc_table;

int init_irc(void)
{
	struct type_s *type;

	add_signal("irc.error", 0);
	init_irc_server();

	ADD_HANDLER_LIST(irc_handlers);

	if (!(type = find_type("table")) || !(irc_table = add_variable(NULL, type, "irc", 0, "")))
		return(-1);

	ADD_VARIABLE_LIST(irc_table, irc_variables);
	ADD_KEY_LIST(irc_keys);

	IRC_INIT_JOINPOINT(irc_table)

	return(0);
}

int release_irc(void)
{
	struct type_s *type;

	IRC_RELEASE_JOINPOINT(irc_table)

	/** Remove the whole variable table */
	if ((type = find_type("table"))) {
		add_variable(NULL, type, "irc", 0, "");
		remove_variable(NULL, type, "irc");
	}

	REMOVE_HANDLER_LIST(irc_handlers);

	release_irc_server();
	remove_signal("irc.error");
	return(0);
}


