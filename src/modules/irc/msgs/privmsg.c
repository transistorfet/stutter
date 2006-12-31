/*
 * Module Name:		privmsg.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Private Message Notification Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Prints private message to channel.
 */
int irc_msg_privmsg(char *env, struct irc_msg *msg)
{
	char buffer[LARGE_STRING_SIZE];
	struct irc_channel *channel;

	if (*msg->text == 0x01)
		emit_signal("irc.dispatch_ctcp", NULL, msg);
	else {
		if (!(channel = irc_find_channel(msg->server->channels, msg->params[0])) && ((strcmp(msg->server->nick, msg->params[0])) || !(channel = irc_find_channel(msg->server->channels, msg->nick)))) {
			if ((msg->params[0][0] == '#') || (msg->params[0][0] == '&') || (msg->params[0][0] == '+')
			    || (msg->params[0][0] == '!') || !(channel = irc_current_channel()))
				return(-1);
			if (irc_format_msg(msg, msg->nick ? IRC_FMT_PRIVATE_MSG : IRC_FMT_PRIVATE_MSG_SELF, buffer, LARGE_STRING_SIZE) < 0)
				return(-1);
		}
		else if (irc_format_msg(msg, msg->nick ? IRC_FMT_PUBLIC_MSG : IRC_FMT_PUBLIC_MSG_SELF, buffer, LARGE_STRING_SIZE) < 0)
			return(-1);
		fe_print(channel->window, buffer);
	}

	return(0);
}


