/*
 * Command Name:	leave.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Leave Channel Command
 */

#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/modules/irc.h>

int irc_cmd_leave(char *env, char *args)
{
	char *name;
	struct irc_server *server;
	struct irc_channel *channel;

	get_param_m(args, name, ' ');
	if (!(server = irc_current_server()))
		return(-1);
	if ((*name != '\0') ? !(channel = irc_find_channel(server->channels, name)) : !(channel = irc_current_channel()))
		return(-1);
	irc_leave_channel(channel->server, channel->name);
	return(0);
}


