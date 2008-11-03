/*
 * Module Name:		kick.c
 * Description:		Kick Notification Message
 */

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

/**
 * Print kick message to channel.
 */
int irc_msg_kick(char *env, struct irc_msg *msg)
{
	char *fmt;
	struct irc_channel *channel;
	char buffer[LARGE_STRING_SIZE];

	if (!(channel = irc_find_channel(&msg->server->channels, msg->params[0])))
		return(-1);

	if (!strcmp_icase(msg->server->nick, msg->params[1])) {
		if (channel->bitflags & IRC_CBF_AUTO_REJOIN)
			irc_join_channel(msg->server, msg->params[0]);
		else {
			irc_remove_channel(&msg->server->channels, msg->params[0]);
			channel = msg->server->status;
		}
		fmt = IRC_FMT_KICK_SELF;
	}
	else {
		irc_remove_user(&channel->users, msg->params[1]);
		fmt = IRC_FMT_KICK;
	}
	if (irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0)
		signal_emit(channel->signal, buffer);
	return(0);
}


