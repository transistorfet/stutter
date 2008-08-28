/*
 * Module Name:		quit.c
 * Description:		Quit Notification Message
 */

#include CONFIG_H
#include <stutter/output.h>
#include <stutter/modules/irc/irc.h>

static int irc_msg_quit_traverse(struct irc_channel *, struct irc_msg *);

/**
 * Reports the user quit in the status window and the current channel.
 */
int irc_msg_quit(char *env, struct irc_msg *msg)
{
	if (!msg->nick)
		return(-1);
	irc_traverse_channel_list(msg->server->channels, (traverse_t) irc_msg_quit_traverse, msg);
	return(0);
}

/**
 * Remove the user according to the given quit message in the given channel.
 * This function is called from inside irc_traverse_channel_list.
 */
static int irc_msg_quit_traverse(struct irc_channel *channel, struct irc_msg *msg)
{
	if (!irc_remove_user(channel->users, msg->nick) || (channel == msg->server->status)) {
		IRC_MSG_QUIT_OUTPUT_JOINPOINT(channel, msg, IRC_FMT_QUIT)
	}
	return(0);
}

