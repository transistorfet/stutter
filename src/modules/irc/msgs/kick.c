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
int irc_msg_kick(struct irc_server *server, struct irc_msg *msg)
{
	void *window;
	char buffer[STRING_SIZE];
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(server->channels, msg->params[0])))
		return(-1);

	if (!strcmp(server->nick, msg->params[1])) {
		if (channel->bitflags & IRC_CBF_AUTO_REJOIN)
			irc_join_channel(server, msg->params[0]);
		else {
			fe_destroy_widget(channel->window);
			irc_remove_channel(server->channels, msg->params[0]);
			channel = server->status;
		}
		if (irc_format_msg(msg, IRC_FMT_KICK_SELF, buffer, STRING_SIZE) < 0)
			return(-1);
	}
	else {
		irc_remove_user(channel->users, msg->params[1]);
		if (irc_format_msg(msg, IRC_FMT_KICK, buffer, STRING_SIZE) < 0)
			return(-1);
	}

	fe_print(channel->window, buffer);
	return(0);
}


