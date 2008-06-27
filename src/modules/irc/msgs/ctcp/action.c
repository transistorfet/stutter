/*
 * Module Name:		action.c
 * Description:		Action CTCP Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/modules/irc.h>

/**
 * Prints private message to channel.
 */
int irc_msg_ctcp_action(char *env, struct irc_msg *msg)
{
	char *fmt;
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(msg->server->channels, msg->params[0]))) {
		if ((msg->params[0][0] == '#') || (msg->params[0][0] == '&') || (msg->params[0][0] == '+')
		    || (msg->params[0][0] == '!') || !(channel = irc_current_channel()))
			return(0);
		fmt = msg->nick ? IRC_FMT_PRIVATE_ACTION : IRC_FMT_PRIVATE_ACTION_SELF;
	}
	else
		fmt = msg->nick ? IRC_FMT_PUBLIC_ACTION : IRC_FMT_PUBLIC_ACTION_SELF;
	IRC_MSG_CTCP_ACTION_OUTPUT_JOINPOINT(channel, msg, fmt)
	return(0);
}


