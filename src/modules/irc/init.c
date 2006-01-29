/*
 * Module Name:		load.c
 * Version:		0.1
 * Module Requirements:	type ; string ; variable ; modirc
 * Description:		IRC Module Initializer
 */

#include <stdlib.h>

#include CONFIG_H
#include <type.h>
#include <variable.h>
#include <lib/string.h>
#include <lib/globals.h>
#include <modules/irc.h>

struct command_prototype_s {
	char *name;
	callback_t func;
	void *ptr;
};

#define DECLARE_COMMAND(name)	\
	{ #name, (callback_t) irc_cmd_##name, NULL }

static struct command_prototype_s irc_commands[] = {
	DECLARE_COMMAND(join),
	DECLARE_COMMAND(leave),
	DECLARE_COMMAND(quit),
	DECLARE_COMMAND(msg),
	DECLARE_COMMAND(me),
	DECLARE_COMMAND(nick),
	DECLARE_COMMAND(names),
	DECLARE_COMMAND(notice),
	DECLARE_COMMAND(say),
	DECLARE_COMMAND(server),
	DECLARE_COMMAND(disconnect),
	DECLARE_COMMAND(whois),
	{ "", (callback_t) irc_cmd_say, NULL },
	{ NULL, NULL, NULL }
};

int init_irc(void)
{
	int i = 0;
	struct type_s *type;

	init_irc_server();

	if (!(type = find_type("status")) || !type->create)
		return(-1);
	add_variable(type, IRC_NAMESPACE, "current_nick", type->create("%r%p", irc_stringify_nick, NULL));
	add_variable(type, IRC_NAMESPACE, "current_channel", type->create("%r%p", irc_stringify_channel, NULL));

	if (!(type = find_type("command")) || !type->create)
		return(-1);
	for (;irc_commands[i].name;i++)
		add_variable(type, IRC_NAMESPACE, irc_commands[i].name, type->create("%r%p", irc_commands[i].func, irc_commands[i].ptr));

	return(0);
}

int release_irc(void)
{
	int i = 0;
	struct type_s *type;

	if (!(type = find_type("command")) || !type->create)
		return(-1);

	for (;irc_commands[i].name;i++)
		remove_variable(type, IRC_NAMESPACE, irc_commands[i].name);

	release_irc_server();
	return(0);
}


