/*
 * Module Name:		ping.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Ping CTCP Message
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_msg_ctcp_ping(char *env, void *index, struct irc_msg *msg)
{
	int pos;
	char *tmp;
	char number[20];
	char buffer[STRING_SIZE];
	struct irc_channel *channel;

	if (!strncmp_icase(&msg->text[1], "PING", 4) && msg->nick) {
		if (!(channel = irc_current_channel()))
			return(-1);
		if (msg->cmd == IRC_MSG_PRIVMSG) {
			irc_notice(msg->server, msg->nick, msg->text);
			if (irc_format_msg(msg, IRC_FMT_PING, buffer, STRING_SIZE) < 0)
				return(-1);
		}
		else if (msg->cmd == IRC_MSG_NOTICE) {
			tmp = msg->text;
			pos = strlen(msg->text) - 1;
			msg->text[pos] = '\0';
			snprintf(number, 20, "%d secs", time(NULL) - atoi(&msg->text[6]));
			msg->text = number;
			if (irc_format_msg(msg, IRC_FMT_PING_REPLY, buffer, STRING_SIZE) < 0)
				return(-1);
			msg->text = tmp;
			msg->text[pos] = 0x01;
		}
		else
			return(-1);
		fe_print(channel->window, buffer);
	}
	return(0);
}


