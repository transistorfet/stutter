/*
 * Command Name:	me.c
 * Version:		0.1
 * Module Requirements:	string ; modirc
 * Description:		Action Message Command
 */

#include <stutter/string.h>
#include <stutter/modules/irc.h>

int irc_cmd_me(char *env, char *args)
{
	char *action;
	struct irc_channel *channel;

	if (*args == '\0' || !(channel = irc_current_channel()))
		return(-1);
	/** The string \x01ACTION is interpreted incorrectly because AC
	    is taken to be another hex number */
	if (!(action = create_string("\x01\x41\x43TION %s\x01", args)))
		return(-1);
	irc_private_msg(channel->server, channel->name, action);
	destroy_string(action);
	return(0);
}


