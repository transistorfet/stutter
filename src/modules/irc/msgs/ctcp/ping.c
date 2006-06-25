/*
 * Module Name:		ping.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Ping CTCP Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Prints private message to channel.
 */
int irc_msg_ctcp_ping(char *env, struct irc_msg *msg)
{
	char buffer[STRING_SIZE];
	struct irc_channel *channel;

	if (!strncmp(&msg->text[1], "PING", 4) && msg->nick) {
		if (!(channel = irc_current_channel()))
			return(-1);
		if (msg->cmd == IRC_MSG_PRIVMSG) {
			irc_notice(msg->server, msg->nick, msg->text);
			if (irc_format_msg(msg, IRC_FMT_PING, buffer, STRING_SIZE) < 0)
				return(-1);
		}
		else if (msg->cmd == IRC_MSG_NOTICE) {
			// TODO format msg->text
			if (irc_format_msg(msg, IRC_FMT_PING_REPLY, buffer, STRING_SIZE) < 0)
				return(-1);
		}
		else
			return(-1);
		fe_print(channel->window, buffer);
	}
	return(0);
}


