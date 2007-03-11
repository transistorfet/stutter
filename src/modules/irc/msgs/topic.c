/*
 * Module Name:		topic.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Topic Change Notification Message
 */

#include CONFIG_H
#include <stutter/string.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Update the channel topic and print message to channel.
 */
int irc_msg_topic(char *env, struct irc_msg *msg)
{
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(msg->server->channels, ((msg->cmd == IRC_MSG_TOPIC) ? msg->params[0] : msg->params[1]))))
		return(-1);

	if (channel->topic)
		destroy_string(channel->topic);
	channel->topic = create_string("%s", msg->text);

	IRC_MSG_TOPIC_OUTPUT_JOINPOINT(channel, msg, IRC_FMT_TOPIC)
	return(0);
}


