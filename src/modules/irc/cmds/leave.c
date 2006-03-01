/*
 * Command Name:	leave.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Leave Channel Command
 */

#include <frontend.h>
#include <lib/macros.h>
#include <modules/irc.h>

int irc_cmd_leave(char *env, char *args)
{
	char *name;
	struct irc_server *server;
	struct irc_channel *channel;

	get_param_m(args, name, ' ');
	if (!(server = irc_current_server()) || !(channel = irc_find_channel(server->channels, name)))
		return(-1);
	irc_leave_channel(channel->server, name);
	return(0);
}


