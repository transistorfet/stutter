/*
 * Module Name:		kick.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Kick Notification Message
 */

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Print kick message to channel.
 */
int irc_msg_kick(char *env, struct irc_msg *msg)
{
	char *fmt;
	void *window;
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(msg->server->channels, msg->params[0])))
		return(-1);

	if (!strcmp(msg->server->nick, msg->params[1])) {
		if (channel->bitflags & IRC_CBF_AUTO_REJOIN)
			irc_join_channel(msg->server, msg->params[0]);
		else {
			irc_remove_channel(msg->server->channels, msg->params[0]);
			channel = msg->server->status;
		}
		fmt = IRC_FMT_KICK_SELF;
	}
	else {
		irc_remove_user(channel->users, msg->params[1]);
		fmt = IRC_FMT_KICK;
	}

	IRC_MSG_KICK_OUTPUT_JOINPOINT(channel, msg, fmt)
	return(0);
}


