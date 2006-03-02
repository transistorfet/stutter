/*
 * Module Name:		join.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Join Channel Notification Message
 */

#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_join(struct irc_server *server, struct irc_msg *msg)
{
	char *str;
	void *window;
	struct irc_channel *channel;

	if (!strcmp(server->nick, msg->nick)) {
		if (!(window = fe_create_widget("irc", "window", NULL, NULL)) || !(channel = irc_add_channel(server->channels, msg->params[0], window, server))) {
			irc_leave_channel(server, msg->params[0]);
			// TODO print error message
			return(-1);
		}
		fe_select_widget("window", NULL, window);
	}
	else if (!(channel = irc_find_channel(server->channels, msg->params[0])))
		return(-1);

	irc_add_user(channel->users, msg->nick, 0);

	if (!(str = irc_format_msg(msg, IRC_FMT_JOIN)))
		return(-1);
	fe_print(channel->window, str);
	return(0);
}


