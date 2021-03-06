/*
 * Module Name:		version.c
 * Description:		Version CTCP Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/variable.h>
#include <stutter/modules/irc/irc.h>

int irc_msg_ctcp_version(char *env, struct irc_msg *msg)
{
	char *fmt;
	char buffer[LARGE_STRING_SIZE];

	if (!msg->nick)
		return(0);
	if (msg->cmd == IRC_MSG_PRIVMSG) {
		if (stringify_variable(NULL, "irc.version", buffer, STRING_SIZE) >= 0)
			irc_ctcp_reply(msg->server, "VERSION", msg->nick, buffer);
		fmt = IRC_FMT_CTCP_VERSION;
	}
	else if (msg->cmd == IRC_MSG_NOTICE)
		fmt = IRC_FMT_CTCP_VERSION_REPLY;
	else
		return(0);
	// TODO fix this to print to the current channel or status or something
	//if (irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0)
	//	signal_emit(channel->signal, buffer);
	return(0);
}


