/*
 * Module Name:		load.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; string ; modirc
 * Description:		IRC Module Initializer
 */

#include <stdlib.h>

#include CONFIG_H
#include <type.h>
#include <variable.h>
#include <nit/string.h>
#include <modules/irc.h>
#include <nit/callback.h>

struct command_prototype_s {
	char *name;
	callback_t func;
	void *ptr;
};

static struct command_prototype_s irc_commands[] = {
	{ "join", (callback_t) irc_cmd_join, NULL },
	{ "leave", (callback_t) irc_cmd_leave, NULL },
	{ "quit", (callback_t) irc_cmd_quit, NULL },
	{ "msg", (callback_t) irc_cmd_msg, NULL },
	{ "me", (callback_t) irc_cmd_me, NULL },
	{ "nick", (callback_t) irc_cmd_nick, NULL },
	{ "notice", (callback_t) irc_cmd_notice, NULL },
	{ "say", (callback_t) irc_cmd_say, NULL },
	{ "", (callback_t) irc_cmd_say, NULL },
	{ NULL, NULL, NULL }
};

int init_irc(void)
{
	int i = 0;
	struct type_s *type;

	init_irc_server();

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


