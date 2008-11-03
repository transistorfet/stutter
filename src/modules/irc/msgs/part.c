/*
 * Module Name:		part.c
 * Description:		Part Channel Notification Message
 */

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_part(char *env, struct irc_msg *msg)
{
	struct irc_channel *channel;
	char buffer[LARGE_STRING_SIZE];

	if (!(channel = irc_find_channel(&msg->server->channels, msg->params[0])))
		return(-1);

	if (!strcmp_icase(msg->server->nick, msg->nick))
		irc_remove_channel(&msg->server->channels, msg->params[0]);
	else {
		irc_remove_user(&channel->users, msg->nick);
		if (irc_format_msg(msg, IRC_FMT_PART, buffer, LARGE_STRING_SIZE) >= 0)
			signal_emit(channel->signal, buffer);
	}
	return(0);
}


