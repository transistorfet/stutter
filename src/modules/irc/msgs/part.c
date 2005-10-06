/*
 * Module Name:		part.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Part Channel Notification Message
 */

#include <frontend.h>
#include <modules/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_part(char *env, struct irc_msg *msg)
{
	char *str;
	struct irc_server *server;
	struct irc_channel *channel;

	if (!(server = irc_current_server()))
		return(-1);
	if (!(channel = irc_get_channel(server, msg->params[0])))
		return(-1);

	irc_remove_user(channel, msg->nick);

	if (!(str = irc_format_msg(msg, IRC_FMT_PART)))
		return(-1);
	fe_print(channel->window, str);
	return(0);
}


