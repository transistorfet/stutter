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
	char buffer[STRING_SIZE];
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(msg->server->channels, msg->params[0])))
		return(-1);

	if (!strcmp(msg->server->nick, msg->nick)) {
		fe_destroy_widget(channel->window);
		irc_remove_channel(msg->server->channels, msg->params[0]);
	}
	else {
		irc_remove_user(channel->users, msg->nick);
		if (irc_format_msg(msg, IRC_FMT_PART, buffer, STRING_SIZE) < 0)
			return(-1);
		fe_print(channel->window, buffer);
	}

	return(0);
}


