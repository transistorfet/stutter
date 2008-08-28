/*
 * Module Name:		nick.c
 * Description:		Nick Change Notification Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/string.h>
#include <stutter/macros.h>
#include <stutter/output.h>
#include <stutter/modules/irc/irc.h>

static int irc_msg_nick_traverse(struct irc_channel *, struct irc_msg *);

/**
 * Update a user's nick and prints message to channel.
 */
int irc_msg_nick(char *env, struct irc_msg *msg)
{
	if (!strcmp_icase(msg->server->nick, msg->nick)) {
		if (msg->server->nick)
			destroy_string(msg->server->nick);
		msg->server->nick = create_string(msg->params[0]);
	}
	irc_traverse_channel_list(msg->server->channels, (traverse_t) irc_msg_nick_traverse, msg);
	return(0);
}

/**
 * Change the nick for the user according to the given nick message in the
 * given channel.  This function is called from inside
 * irc_traverse_channel_list.
 */
static int irc_msg_nick_traverse(struct irc_channel *channel, struct irc_msg *msg)
{
	if (!irc_change_user_nick(channel->users, msg->nick, msg->params[0]) || (channel == msg->server->status)) {
		IRC_MSG_NICK_OUTPUT_JOINPOINT(channel, msg, IRC_FMT_NICK)
	}
	return(0);
}

