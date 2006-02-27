/*
 * Module Name:		nick.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Nick Change Notification Message
 */

#include <frontend.h>
#include <lib/string.h>
#include <modules/irc.h>

static int irc_msg_nick_traverse(struct irc_channel *, struct irc_msg *);

/**
 * Update a user's nick and prints message to channel.
 */
int irc_msg_nick(struct irc_server *server, struct irc_msg *msg)
{
	if (!strcmp(server->nick, msg->nick)) {
		strncpy(server->nick, msg->params[0], IRC_MAX_NICK - 1);
		server->nick[IRC_MAX_NICK - 1] = '\0';
	}
	irc_traverse_channel_list(server->channels, (traverse_t) irc_msg_nick_traverse, msg);
	fe_print(server->status->window, irc_format_msg(msg, IRC_FMT_NICK));
	return(0);
}

/**
 * Change the nick for the user according to the given nick message in the
 * given channel.  This function is called from inside
 * irc_traverse_channel_list.
 */
static int irc_msg_nick_traverse(struct irc_channel *channel, struct irc_msg *msg)
{
	if (!irc_change_user_nick(channel->users, msg->nick, msg->params[0]))
		fe_print(channel->window, irc_format_msg(msg, IRC_FMT_NICK));
}

