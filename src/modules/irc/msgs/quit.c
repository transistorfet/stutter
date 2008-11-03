/*
 * Module Name:		quit.c
 * Description:		Quit Notification Message
 */

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

/**
 * Reports the user quit in the status window and the current channel.
 */
int irc_msg_quit(char *env, struct irc_msg *msg)
{
	struct irc_channel *cur;
	char buffer[LARGE_STRING_SIZE];

	if (!msg->nick)
		return(-1);
	for (cur = msg->server->channels.head; cur; cur = cur->next) {
		if ((!irc_remove_user(&cur->users, msg->nick) || (cur == msg->server->status))
		    && irc_format_msg(msg, IRC_FMT_QUIT, buffer, LARGE_STRING_SIZE) >= 0)
			signal_emit(cur->signal, buffer);
	}
	return(0);
}

