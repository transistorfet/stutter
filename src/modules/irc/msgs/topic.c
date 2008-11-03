/*
 * Module Name:		topic.c
 * Description:		Topic Change Notification Message
 */

#include CONFIG_H
#include <stutter/string.h>
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

/**
 * Update the channel topic and print message to channel.
 */
int irc_msg_topic(char *env, struct irc_msg *msg)
{
	struct irc_channel *channel;
	char buffer[LARGE_STRING_SIZE];

	if (!(channel = irc_find_channel(&msg->server->channels, ((msg->cmd == IRC_MSG_TOPIC) ? msg->params[0] : msg->params[1]))))
		return(-1);

	if (channel->topic)
		destroy_string(channel->topic);
	channel->topic = create_string("%s", msg->text);

	if (irc_format_msg(msg, IRC_FMT_TOPIC, buffer, LARGE_STRING_SIZE) >= 0)
		signal_emit(channel->signal, buffer);
	return(0);
}


