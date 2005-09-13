/*
 * Command Name:	say.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		Say Message Command
 */

#include <modules/irc.h>

int irc_cmd_say(char *env, char *str)
{
	struct irc_channel *channel;

	if (*str == '\0' || !(channel = irc_current_channel()))
		return(-1);
	irc_private_msg(channel->server, channel->name, str);
	return(0);
}


