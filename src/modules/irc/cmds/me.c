/*
 * Command Name:	me.c
 * Version:		0.1
 * Module Requirements:	string ; modirc
 * Description:		Action Message Command
 */


#include <nit/string.h>
#include <modules/irc.h>

int irc_cmd_me(char *env, char *str)
{
	char *action;
	struct irc_channel *channel;

	if (*str == '\0' || !(channel = irc_current_channel()))
		return(-1);
	if (!(action = create_string("\x01\x41\x43TION %s\x01", str)))
		return(-1);
	irc_private_msg(channel->server, channel->name, action);
	destroy_string(action);
	return(0);
}


