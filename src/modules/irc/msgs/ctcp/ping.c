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
	char number[20];
	char *fmt, *tmp = NULL;

	if (!strcmp_icase(msg->ctcps[0].tag, "PING")) {
		if (!msg->nick)
			return(SIGNAL_STOP_EMIT);
		if (msg->cmd == IRC_MSG_PRIVMSG) {
			irc_ctcp_reply(msg->server, "PING", msg->nick, msg->ctcps[0].args);
			fmt = IRC_FMT_PING;
		}
		else if ((msg->cmd == IRC_MSG_NOTICE) && (msg->ctcps[0].args)) {
			tmp = msg->ctcps[0].args;
			msg->ctcps[0].args = number;
			snprintf(number, 20, "%lu secs", time(NULL) - atoi(tmp));
			fmt = IRC_FMT_PING_REPLY;
		}
		else
			return(SIGNAL_STOP_EMIT);
		IRC_MSG_CTCP_PING_OUTPUT_JOINPOINT(msg, fmt)
		if (tmp)
			msg->ctcps[0].args = tmp;
		return(SIGNAL_STOP_EMIT);
	}
	return(0);
}


