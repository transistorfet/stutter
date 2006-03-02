/*
 * Module Name:		topic.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Topic Change Notification Message
 */

#include <stutter/frontend.h>
#include <stutter/lib/string.h>
#include <stutter/modules/irc.h>

/**
 * Update the channel topic and print message to channel.
 */
int irc_msg_topic(struct irc_server *server, struct irc_msg *msg)
{
	char *str;
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(server->channels, ((msg->cmd == IRC_MSG_TOPIC) ? msg->params[0] : msg->params[1]))))
		return(-1);

	if (channel->topic)
		destroy_string(channel->topic);
	channel->topic = create_string(msg->text);

	if (!(str = irc_format_msg(msg, IRC_FMT_TOPIC)))
		return(-1);
	fe_print(channel->window, str);
	return(0);
}


