/*
 * Module Name:		quit.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Quit Notification Message
 */

#include <frontend.h>
#include <nit/string.h>
#include <modules/irc.h>

/**
 * Reports the user quit in the status window and the current channel.
 */
int irc_msg_quit(char *env, struct irc_msg *msg)
{
	char *str;
	struct irc_user *user;
	struct irc_server *server;
	struct irc_channel *channel;

	if (!(server = irc_current_server()))
		return(-1);

	if (!(str = irc_format_msg(msg, IRC_FMT_QUIT)))
		return(-1);
	list_clear_current(server->channels);
	while (channel = list_next(server->channels)) {
		if (user = irc_get_user(channel, msg->nick)) {
			irc_remove_user(channel, msg->nick);
			fe_print(channel->window, duplicate_string(str));
		}
	}
	fe_print(server->status->window, str);
	return(0);
}


