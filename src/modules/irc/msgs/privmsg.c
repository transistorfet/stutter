/*
 * Module Name:		privmsg.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Private Message Notification Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Prints private message to channel.
 */
int irc_msg_privmsg(char *env, struct irc_msg *msg)
{
	char buffer[STRING_SIZE];
	struct irc_channel *channel;

	// TODO modify so privmsgs to individuals are still printed but using a differnt format
	if (!(channel = irc_find_channel(msg->server->channels, msg->params[0])) && ((msg->params[0][0] != '#') && !(channel = irc_current_channel())))
		return(-1);

	if ((msg->text[0] == 0x01) && !strncmp(&msg->text[1], "ACTION", 6)) {
		msg->text = &msg->text[8];
		msg->text[strlen(msg->text) - 1] = '\0';
		if (irc_format_msg(msg, msg->nick ? IRC_FMT_ACTION : IRC_FMT_ACTION_SELF, buffer, STRING_SIZE) < 0)
			return(-1);
	}
	else {
		if (irc_format_msg(msg, msg->nick ? IRC_FMT_PRIVMSG : IRC_FMT_PRIVMSG_SELF, buffer, STRING_SIZE) < 0)
			return(-1);
	}

	fe_print(channel->window, buffer);
	return(0);
}


