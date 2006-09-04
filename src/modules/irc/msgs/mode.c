/*
 * Module Name:		mode.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Mode Change Notification Message
 */

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/lib/string.h>
#include <stutter/modules/irc.h>

int irc_msg_mode(char *env, struct irc_msg *msg)
{
	char buffer[STRING_SIZE];
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(msg->server->channels, msg->params[0])))
		return(-1);
	// TODO update channel and user bitflags
	if (irc_format_msg(msg, IRC_FMT_MODE, buffer, STRING_SIZE) < 0)
		return(-1);
	fe_print(channel->window, buffer);
	return(0);
}


