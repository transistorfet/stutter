/*
 * Command Name:	me.c
 * Description:		Action Message Command
 */

#include <stutter/string.h>
#include <stutter/modules/irc.h>

int irc_cmd_me(char *env, char *args)
{
	struct irc_channel *channel;

	if (*args == '\0' || !(channel = irc_current_channel()))
		return(-1);
	irc_ctcp_msg(channel->server, "ACTION", channel->name, args);
	return(0);
}


