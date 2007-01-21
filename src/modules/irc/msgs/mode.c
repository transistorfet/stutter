/*
 * Module Name:		mode.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Mode Change Notification Message
 */

#include CONFIG_H
#include <stutter/string.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_msg_mode(char *env, struct irc_msg *msg)
{
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(msg->server->channels, msg->params[0])))
		return(-1);
	// TODO update channel and user bitflags
	IRC_MSG_MODE_OUTPUT_JOINPOINT(channel, msg, IRC_FMT_MODE)
	return(0);
}


