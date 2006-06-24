/*
 * Module Name:		quit.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Quit Notification Message
 */

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

static int irc_msg_quit_traverse(struct irc_channel *, struct irc_msg *);

/**
 * Reports the user quit in the status window and the current channel.
 */
int irc_msg_quit(char *env, struct irc_msg *msg)
{
	char buffer[STRING_SIZE];

	irc_traverse_channel_list(msg->server->channels, (traverse_t) irc_msg_quit_traverse, msg);
	if (irc_format_msg(msg, IRC_FMT_QUIT, buffer, STRING_SIZE) < 0)
		return(-1);
	fe_print(msg->server->status->window, buffer);
	return(0);
}

/**
 * Remove the user according to the given quit message in the given channel.
 * This function is called from inside irc_traverse_channel_list.
 */
static int irc_msg_quit_traverse(struct irc_channel *channel, struct irc_msg *msg)
{
	char buffer[STRING_SIZE];

	if (!irc_remove_user(channel->users, msg->nick) && (irc_format_msg(msg, IRC_FMT_QUIT, buffer, STRING_SIZE) >= 0))
		fe_print(channel->window, buffer);
	return(0);
}

