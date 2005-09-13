/*
 * Module Name:		join.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Join Channel Notification Message
 */

#include FRONTEND_H
#include <modules/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_join(char *env, struct irc_msg *msg)
{
	char *str;
	struct irc_server *server;
	struct irc_channel *channel;

	if (!(server = irc_current_server()))
		return(-1);
	if (!(channel = irc_get_channel(server, msg->params[0])))
		return(-1);

	irc_add_user(channel, msg->nick, 0);

	if (!(str = irc_format_msg(msg, IRC_FMT_JOIN)))
		return(-1);
	fe_print(channel->window, str);
	return(0);
}


