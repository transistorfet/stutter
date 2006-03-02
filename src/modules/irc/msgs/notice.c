/*
 * Module Name:		notice.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Notice Notification Message
 */

#include <stutter/frontend.h>
#include <stutter/lib/string.h>
#include <stutter/modules/irc.h>

/**
 * Prints message to current channel and status channel.
 */
int irc_msg_notice(struct irc_server *server, struct irc_msg *msg)
{
	char *str;
	struct irc_channel *channel;

	if (!(channel = irc_current_channel()))
		return(-1);

	if (!(str = irc_format_msg(msg, msg->nick ? IRC_FMT_NOTICE : IRC_FMT_NOTICE_SELF)))
		return(-1);
	if (channel != server->status)
		fe_print(server->status->window, duplicate_string(str));
	fe_print(channel->window, str);
	return(0);
}


