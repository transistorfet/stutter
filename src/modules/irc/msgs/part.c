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
int irc_msg_part(struct irc_server *server, struct irc_msg *msg)
{
	char *str;
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(server->channels, msg->params[0])))
		return(-1);

	if (!strcmp(server->nick, msg->nick)) {
		fe_destroy_widget(channel->window);
		irc_remove_channel(server->channels, msg->params[0]);
	}
	else {
		irc_remove_user(channel->users, msg->nick);
		if (!(str = irc_format_msg(msg, IRC_FMT_PART)))
			return(-1);
		fe_print(channel->window, str);
	}

	return(0);
}


