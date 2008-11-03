/*
 * Module Name:		notice.c
 * Description:		Notice Notification Message
 */

#include CONFIG_H
#include <stutter/string.h>
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

/**
 * Prints message to current channel and status channel.
 */
int irc_msg_notice(char *env, struct irc_msg *msg)
{
	struct irc_channel *cur;
	char buffer[LARGE_STRING_SIZE];

	if (msg->num_ctcps) {
		irc_dispatch_ctcp_msg(msg);
		return(0);
	}

	if (irc_format_msg(msg, msg->nick ? IRC_FMT_NOTICE : IRC_FMT_NOTICE_SELF, buffer, LARGE_STRING_SIZE) < 0)
		return(-1);
	for (cur = msg->server->channels.head; cur; cur = cur->next)
		signal_emit(cur->signal, buffer);
	return(0);
}


