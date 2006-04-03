/*
 * Module Name:		privmsg.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Private Message Notification Message
 */

#include <string.h>

#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Prints private message to channel.
 */
int irc_msg_privmsg(struct irc_server *server, struct irc_msg *msg)
{
	char *str, *format;
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(server->channels, msg->params[0])) && ((msg->params[0][0] != '#') && !(channel = irc_current_channel())))
		return(-1);

	if ((msg->text[0] == 0x01) && !strncmp(&msg->text[1], "ACTION", 6)) {
		msg->text = &msg->text[8];
		msg->text[strlen(msg->text) - 1] = '\0';
		format = msg->nick ? IRC_FMT_ACTION : IRC_FMT_ACTION_SELF;
	}
	else
		format = msg->nick ? IRC_FMT_PRIVMSG : IRC_FMT_PRIVMSG_SELF;

	if (!(str = irc_format_msg(msg, format)))
		return(-1);
	fe_print(channel->window, str);
	return(0);
}


