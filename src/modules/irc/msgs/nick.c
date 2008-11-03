/*
 * Module Name:		nick.c
 * Description:		Nick Change Notification Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/string.h>
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

/**
 * Update a user's nick and prints message to channel.
 */
int irc_msg_nick(char *env, struct irc_msg *msg)
{
	struct irc_channel *cur;
	char buffer[LARGE_STRING_SIZE];

	if (!strcmp_icase(msg->server->nick, msg->nick)) {
		if (msg->server->nick)
			destroy_string(msg->server->nick);
		msg->server->nick = create_string(msg->params[0]);
	}

	if (irc_format_msg(msg, IRC_FMT_NICK, buffer, LARGE_STRING_SIZE) < 0)
		return(-1);
	for (cur = msg->server->channels.head; cur; cur = cur->next) {
		if (!irc_change_user_nick(&cur->users, msg->nick, msg->params[0])
		    || (cur == msg->server->status))
			signal_emit(cur->signal, buffer);
	}
	return(0);
}


