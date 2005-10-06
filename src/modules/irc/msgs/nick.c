/*
 * Module Name:		nick.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Nick Change Notification Message
 */

#include <frontend.h>
#include <nit/string.h>
#include <modules/irc.h>

/**
 * Update a user's nick and prints message to channel.
 */
int irc_msg_nick(char *env, struct irc_msg *msg)
{
	char *str;
	struct irc_user *user;
	struct irc_server *server;
	struct irc_channel *channel;

	if (!(server = irc_current_server()))
		return(-1);

	if (!(str = irc_format_msg(msg, IRC_FMT_NICK)))
		return(-1);
	list_clear_current(server->channels);
	while (channel = list_next(server->channels)) {
		if (user = irc_get_user(channel, msg->nick)) {
			irc_change_user_nick(user, msg->params[0]);
			fe_print(channel->window, duplicate_string(str));
		}
	}

	fe_print(server->status->window, str);
	return(0);
}


