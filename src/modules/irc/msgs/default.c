/*
 * Module Name:		default.c
 * Description:		Default Notification Message (for Unhandled Messages)
 */

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

int irc_msg_default(char *env, struct irc_msg *msg)
{
	char buffer[LARGE_STRING_SIZE];

	if (irc_format_msg(msg, env ? env : IRC_FMT_DEFAULT, buffer, LARGE_STRING_SIZE) >= 0)
		signal_emit(msg->server->status->signal, buffer);
	return(0);
}


