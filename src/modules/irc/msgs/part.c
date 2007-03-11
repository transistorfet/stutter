/*
 * Module Name:		part.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Part Channel Notification Message
 */

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_part(char *env, struct irc_msg *msg)
{
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(msg->server->channels, msg->params[0])))
		return(-1);

	if (!strcmp(msg->server->nick, msg->nick))
		irc_remove_channel(msg->server->channels, msg->params[0]);
	else {
		irc_remove_user(channel->users, msg->nick);
		IRC_MSG_PART_OUTPUT_JOINPOINT(channel, msg, IRC_FMT_PART)
	}
	return(0);
}


