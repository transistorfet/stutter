/*
 * Module Name:		ping.c
 * Description:		Ping CTCP Message
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/output.h>
#include <stutter/modules/irc/irc.h>

int irc_msg_ctcp_ping(char *env, struct irc_msg *msg)
{
	char number[20];
	char *fmt, *tmp = NULL;

	if (!msg->nick)
		return(0);
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
		return(0);
	IRC_MSG_CTCP_PING_OUTPUT_JOINPOINT(msg, fmt)
	if (tmp)
		msg->ctcps[0].args = tmp;
	return(0);
}


