/*
 * Module Name:		nick.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Nick Change Notification Message
 */

#include <stutter/frontend.h>
#include <stutter/lib/string.h>
#include <stutter/modules/irc.h>

static int irc_msg_nick_traverse(struct irc_channel *, struct irc_msg *);

/**
 * Update a user's nick and prints message to channel.
 */
int irc_msg_nick(char *env, struct irc_msg *msg)
{
	char buffer[STRING_SIZE];

	if (!strcmp(msg->server->nick, msg->nick)) {
		strncpy(msg->server->nick, msg->params[0], IRC_MAX_NICK - 1);
		msg->server->nick[IRC_MAX_NICK - 1] = '\0';
	}
	irc_traverse_channel_list(msg->server->channels, (traverse_t) irc_msg_nick_traverse, msg);

	if (irc_format_msg(msg, IRC_FMT_NICK, buffer, STRING_SIZE) < 0)
		return(-1);
	fe_print(msg->server->status->window, buffer);
	return(0);
}

/**
 * Change the nick for the user according to the given nick message in the
 * given channel.  This function is called from inside
 * irc_traverse_channel_list.
 */
static int irc_msg_nick_traverse(struct irc_channel *channel, struct irc_msg *msg)
{
	char buffer[STRING_SIZE];

	if (!irc_change_user_nick(channel->users, msg->nick, msg->params[0]) && (irc_format_msg(msg, IRC_FMT_NICK, buffer, STRING_SIZE) >= 0))
		fe_print(channel->window, buffer);
	return(0);
}

