/*
 * Module Name:		join.c
 * Version:		0.1
 * Module Requirements:	utils ; frontend ; modirc
 * Description:		Join Channel Notification Message
 */

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_join(char *env, struct irc_msg *msg)
{
	void *window, *frame;
	struct irc_channel *channel;

	if (!strcmp(msg->server->nick, msg->nick)) {
		if (channel = irc_find_channel(msg->server->channels, msg->params[0]))
			fe_select_widget("text", NULL, channel->window);
		else if ((frame = fe_current_widget("frame", NULL))
		    && (window = fe_create_widget("irc", "text", msg->params[0], frame))
		    && (channel = irc_add_channel(msg->server->channels, msg->params[0], window, msg->server)))
			fe_select_widget("text", NULL, window);
		else {
			irc_leave_channel(msg->server, msg->params[0]);
			if (!channel && window)
				fe_destroy_widget(window);
			IRC_ERROR_JOINPOINT(IRC_ERR_JOIN_ERROR, msg->params[0])
			return(-1);
		}
	}
	else if (!(channel = irc_find_channel(msg->server->channels, msg->params[0])))
		return(-1);

	irc_add_user(channel->users, msg->nick, 0);

	IRC_MSG_JOIN_OUTPUT_JOINPOINT(channel, msg, IRC_FMT_JOIN)
	return(0);
}


