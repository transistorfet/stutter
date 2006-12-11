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
#include <stutter/variable.h>
#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>
#include <stutter/modules/irc.h>

DEFINE_HANDLER_LIST(irc_handlers,
	IRC_HANDLERS()
);

DEFINE_COMMAND_LIST(irc_commands,
	IRC_COMMANDS()
);

DEFINE_KEY_LIST(irc_keys,
	IRC_BINDINGS()
);

static struct variable_s *irc_table;

int init_irc(void)
{
	int i = 0;
	struct type_s *type;

	add_signal("irc.error", 0);
	init_irc_server();

	ADD_HANDLER_LIST(irc_handlers);

	if (!(type = find_type("table")) || !(irc_table = add_variable(NULL, type, "irc", 0, "")))
		return(-1);

	if (!(type = find_type("string")) || !type->create)
		return(-1);
	add_variable(irc_table->value, type, "version", 0, "%s", IRC_VERSION_RESPONSE);

	if (!(type = find_type("status")) || !type->create)
		return(-1);
	add_variable(irc_table->value, type, "current_nick", 0, "%r%p", irc_stringify_nick, NULL);
	add_variable(irc_table->value, type, "current_channel", 0, "%r%p", irc_stringify_channel, NULL);

	ADD_COMMAND_LIST(irc_table, irc_commands);
	ADD_KEY_LIST(irc_keys);

	IRC_INIT_JOINPOINT(irc_table)

	return(0);
}

int release_irc(void)
{
	int i = 0;
	struct type_s *type;

	IRC_RELEASE_JOINPOINT(irc_table)

	/** Remove the whole variable table */
	irc_table->bitflags &= ~VAR_BF_NO_REMOVE;
	remove_variable(NULL, NULL, "irc");

	REMOVE_HANDLER_LIST(irc_handlers);

	release_irc_server();
	remove_signal("irc.error");
	return(0);
}


