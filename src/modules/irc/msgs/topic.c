/*
 * Module Name:		topic.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Topic Change Notification Message
 */

#include FRONTEND_H
#include <nit/string.h>
#include <modules/irc.h>

/**
 * Update the channel topic and print message to channel.
 */
int irc_msg_topic(char *env, struct irc_msg *msg)
{
	char *str;
	struct irc_server *server;
	struct irc_channel *channel;

	if (!(server = irc_current_server()))
		return(-1);
	if (!(channel = irc_get_channel(server, ((msg->cmd == IRC_MSG_TOPIC) ? msg->params[0] : msg->params[1]))))
		return(-1);

	if (channel->topic)
		destroy_string(channel->topic);
	channel->topic = create_string(msg->text);

	if (!(str = irc_format_msg(msg, IRC_FMT_TOPIC)))
		return(-1);
	fe_print(channel->window, str);
	return(0);
}


