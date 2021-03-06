/*
 * Module Name:		ping.c
 * Description:		Ping CTCP Message
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

int irc_msg_ctcp_ping(char *env, struct irc_msg *msg)
{
	char number[20];
	char *fmt, *tmp = NULL;
	char buffer[LARGE_STRING_SIZE];

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
	// TODO fix this to send to an appropriate signal
	if (irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0)
		signal_emit(msg->server->status->signal, buffer);
	if (tmp)
		msg->ctcps[0].args = tmp;
	return(0);
}


