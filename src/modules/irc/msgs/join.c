/*
 * Module Name:		join.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Join Channel Notification Message
 */

#include <frontend.h>
#include <modules/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_join(struct irc_server *server, struct irc_msg *msg)
{
	char *str;
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(server->channels, msg->params[0])))
		return(-1);

	irc_add_user(channel->users, msg->nick, 0);

	if (!(str = irc_format_msg(msg, IRC_FMT_JOIN)))
		return(-1);
	fe_print(channel->window, str);
	return(0);
}


