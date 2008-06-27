/*
 * Module Name:		privmsg.c
 * Description:		Private Message Notification Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/modules/irc.h>

/**
 * Prints private message to channel.
 */
int irc_msg_privmsg(char *env, struct irc_msg *msg)
{
	char *fmt;
	struct irc_channel *channel;

	if (msg->num_ctcps)
		emit_signal(NULL, "irc.dispatch_ctcp", msg);
	else {
		if (!(channel = irc_find_channel(msg->server->channels, msg->params[0])) && ((strcmp_icase(msg->server->nick, msg->params[0])) || !msg->nick || !(channel = irc_find_channel(msg->server->channels, msg->nick)))) {
			if ((msg->params[0][0] == '#') || (msg->params[0][0] == '&') || (msg->params[0][0] == '+')
			    || (msg->params[0][0] == '!') || !(channel = irc_current_channel()))
				return(-1);
			fmt = msg->nick ? IRC_FMT_PRIVATE_MSG : IRC_FMT_PRIVATE_MSG_SELF;
		}
		else
			fmt = msg->nick ? IRC_FMT_PUBLIC_MSG : IRC_FMT_PUBLIC_MSG_SELF;
		IRC_MSG_PRIVMSG_OUTPUT_JOINPOINT(channel, msg, fmt)
	}

	return(0);
}


