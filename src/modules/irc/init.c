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

static struct handler_prototype_s irc_handlers[] = {
	IRC_HANDLERS()
	{ NULL, NULL, 0, NULL, NULL }
};

static struct command_prototype_s irc_commands[] = {
	IRC_COMMANDS()
	{ NULL, NULL, NULL }
};

static struct variable_s *irc_table;

int init_irc(void)
{
	int i = 0;
	struct type_s *type;

	add_signal("irc.error", 0);
	init_irc_server();

	for (i = 0;irc_handlers[i].name;i++)
		signal_connect(irc_handlers[i].name, irc_handlers[i].index, irc_handlers[i].priority, (signal_t) irc_handlers[i].func, irc_handlers[i].ptr);

	if (!(type = find_type("table")) || !(irc_table = add_variable(NULL, type, "irc", 0, "")))
		return(-1);

	if (!(type = find_type("string")) || !type->create)
		return(-1);
	add_variable(irc_table->value, type, "version", 0, "%s", IRC_VERSION_RESPONSE);

	if (!(type = find_type("status")) || !type->create)
		return(-1);
	add_variable(irc_table->value, type, "current_nick", 0, "%r%p", irc_stringify_nick, NULL);
	add_variable(irc_table->value, type, "current_channel", 0, "%r%p", irc_stringify_channel, NULL);

	if (!(type = find_type("command")) || !type->create)
		return(-1);
	for (i = 0;irc_commands[i].name;i++)
		add_variable(irc_table->value, type, irc_commands[i].name, 0, "%r%p", irc_commands[i].func, irc_commands[i].ptr);

	return(0);
}

int release_irc(void)
{
	int i = 0;
	struct type_s *type;

	if (!(type = find_type("command")) || !type->create)
		return(-1);

	for (i = 0;irc_commands[i].name;i++)
		remove_variable(irc_table->value, type, irc_commands[i].name);

	for (i = 0;irc_handlers[i].name;i++)
		signal_disconnect(irc_handlers[i].name, irc_handlers[i].index, (signal_t) irc_handlers[i].func, irc_handlers[i].ptr);

	release_irc_server();
	remove_signal("irc.error");
	return(0);
}


