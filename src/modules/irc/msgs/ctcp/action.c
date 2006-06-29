/*
 * Module Name:		action.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Action CTCP Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Prints private message to channel.
 */
int irc_msg_ctcp_action(char *env, struct irc_msg *msg)
{
	char buffer[STRING_SIZE];
	struct irc_channel *channel;

	if (!strncmp(&msg->text[1], "ACTION", 6)) {
		msg->text = &msg->text[8];
		msg->text[strlen(msg->text) - 1] = '\0';
		if (!(channel = irc_find_channel(msg->server->channels, msg->params[0]))) {
			if ((msg->params[0][0] == '#') || !(channel = irc_current_channel()))
				return(-1);
			if (irc_format_msg(msg, msg->nick ? IRC_FMT_PRIVATE_ACTION : IRC_FMT_PRIVATE_ACTION_SELF, buffer, STRING_SIZE) < 0)
				return(-1);
		}
		else if (irc_format_msg(msg, msg->nick ? IRC_FMT_PUBLIC_ACTION : IRC_FMT_PUBLIC_ACTION_SELF, buffer, STRING_SIZE) < 0)
			return(-1);
		fe_print(channel->window, buffer);
	}
	return(0);
}

