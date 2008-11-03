/*
 * Module Name:		privmsg.c
 * Description:		Private Message Notification Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

/**
 * Prints private message to channel.
 */
int irc_msg_privmsg(char *env, struct irc_msg *msg)
{
	char *fmt;
	struct irc_channel *channel;
	char buffer[LARGE_STRING_SIZE];

	if (msg->num_ctcps) {
		irc_dispatch_ctcp_msg(msg);
		return(0);
	}

	if (!(channel = irc_find_channel(&msg->server->channels, msg->params[0]))
	    && (strcmp_icase(msg->server->nick, msg->params[0])
	    || !msg->nick || !(channel = irc_find_channel(&msg->server->channels, msg->nick)))) {
		// TODO this will cause private actions to be printed to status but this should really
		//	be sent to some kind of signal that will be printed to the current window
		channel = msg->server->status;
		fmt = msg->nick ? IRC_FMT_PRIVATE_MSG : IRC_FMT_PRIVATE_MSG_SELF;
	}
	else
		fmt = msg->nick ? IRC_FMT_PUBLIC_MSG : IRC_FMT_PUBLIC_MSG_SELF;
	if (irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0)
		signal_emit(channel->signal, buffer);
	return(0);
}


