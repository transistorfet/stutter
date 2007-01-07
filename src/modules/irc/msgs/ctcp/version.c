/*
 * Module Name:		version.c
 * Version:		0.1
 * Module Requirements:	frontend ; variable ; modirc
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
	int i;
	char buffer[STRING_SIZE];
	struct irc_channel *channel;

	if (!strncmp_icase(&msg->text[1], "VERSION", 7)) {
		if (!msg->nick)
			return(SIGNAL_STOP_EMIT);
		if (!(channel = irc_current_channel()))
			return(SIGNAL_STOP_EMIT);
		if (msg->cmd == IRC_MSG_PRIVMSG) {
			if (util_expand_str("\x01VERSION $irc.version\x01", buffer, STRING_SIZE) >= 0)
				irc_notice(msg->server, msg->nick, buffer);
			if (irc_format_msg(msg, IRC_FMT_CTCP_VERSION, buffer, STRING_SIZE) < 0)
				return(SIGNAL_STOP_EMIT);
		}
		else if (msg->cmd == IRC_MSG_NOTICE) {
			msg->text[strlen(msg->text) - 1] = '\0';
			msg->text = &msg->text[9];
			if (irc_format_msg(msg, IRC_FMT_CTCP_VERSION_REPLY, buffer, STRING_SIZE) < 0)
				return(SIGNAL_STOP_EMIT);
		}
		else
			return(SIGNAL_STOP_EMIT);
		fe_print(channel->window, buffer);
		return(SIGNAL_STOP_EMIT);
	}
	return(0);
}


