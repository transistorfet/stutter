/*
 * Module Name:		quit.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Quit Notification Message
 */

#include <frontend.h>
#include <nit/string.h>
#include <modules/irc.h>

static int irc_msg_quit_traverse(struct irc_channel *, struct irc_msg *);

/**
 * Reports the user quit in the status window and the current channel.
 */
int irc_msg_quit(char *env, struct irc_msg *msg)
{
	struct irc_server *server;

	if (!(server = irc_current_server()))
		return(-1);
	irc_traverse_channel_list(server->channels, (traverse_t) irc_msg_quit_traverse, msg);
	fe_print(server->status->window, irc_format_msg(msg, IRC_FMT_QUIT));
	return(0);
}

/**
 * Remove the user according to the given quit message in the given channel.
 * This function is called from inside irc_traverse_channel_list.
 */
static int irc_msg_quit_traverse(struct irc_channel *channel, struct irc_msg *msg)
{
	if (!irc_remove_user(channel->users, msg->nick))
		fe_print(channel->window, irc_format_msg(msg, IRC_FMT_QUIT));
}

