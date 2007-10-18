/*
 * Module Name:		version.c
 * Description:		Version CTCP Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/variable.h>
#include <stutter/modules/irc.h>

int irc_msg_ctcp_version(char *env, void *index, struct irc_msg *msg)
{
	char *fmt;
	char buffer[STRING_SIZE];

	if (!strcmp_icase(msg->ctcps[0].tag, "VERSION")) {
		if (!msg->nick)
			return(SIGNAL_STOP_EMIT);
		if (msg->cmd == IRC_MSG_PRIVMSG) {
			if (stringify_variable(NULL, "irc.version", buffer, STRING_SIZE) >= 0)
				irc_ctcp_reply(msg->server, "VERSION", msg->nick, buffer);
			fmt = IRC_FMT_CTCP_VERSION;
		}
		else if (msg->cmd == IRC_MSG_NOTICE)
			fmt = IRC_FMT_CTCP_VERSION_REPLY;
		else
			return(SIGNAL_STOP_EMIT);
		IRC_MSG_CTCP_VERSION_OUTPUT_JOINPOINT(msg, fmt)
		return(SIGNAL_STOP_EMIT);
	}
	return(0);
}


