/*
 * Module Name:		kick.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Kick Notification Message
 */

#include FRONTEND_H
#include <modules/irc.h>

/**
 * Print kick message to channel.
 */
int irc_msg_kick(char *env, struct irc_msg *msg)
{
	void *window;
	char *str, *format;
	struct irc_server *server;
	struct irc_channel *channel;

	if (!(server = irc_current_server()))
		return(-1);
	if (!(channel = irc_get_channel(server, msg->params[0])))
		return(-1);

	if (!strcmp(server->nick, msg->params[1])) {
		format = IRC_FMT_KICK_SELF;
		if (channel->bitflags & IRC_CBF_AUTO_REJOIN) {
			window = channel->window;
			list_delete(server->channels, msg->params[0]);
			irc_join_channel(server, msg->params[0], window);
		}
		else {
			fe_destroy_widget(channel->window);
			list_delete(server->channels, msg->params[0]);
			channel = server->status;
		}
	}
	else {
		irc_remove_user(channel, msg->params[1]);
		format = IRC_FMT_KICK;
	}

	if (!(str = irc_format_msg(msg, format)))
		return(-1);
	fe_print(channel->window, str);
	return(0);
}


