/*
 * Module Name:		notice.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Notice Notification Message
 */

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/lib/string.h>
#include <stutter/modules/irc.h>

/**
 * Prints message to current channel and status channel.
 */
int irc_msg_notice(char *env, struct irc_msg *msg)
{
	char buffer[LARGE_STRING_SIZE];
	struct irc_channel *channel;

	if (*msg->text == 0x01)
		emit_signal("irc.dispatch_ctcp", NULL, msg);
	else {
		if (!(channel = irc_current_channel()))
			return(-1);
		if (irc_format_msg(msg, msg->nick ? IRC_FMT_NOTICE : IRC_FMT_NOTICE_SELF, buffer, LARGE_STRING_SIZE) < 0)
			return(-1);
		if (channel != msg->server->status)
			fe_print(msg->server->status->window, buffer);
		fe_print(channel->window, buffer);
	}
	return(0);
}


