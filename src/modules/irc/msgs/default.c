/*
 * Module Name:		default.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Default Notification Message (for Unhandled Messages)
 */

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_msg_default(char *env, struct irc_msg *msg)
{
	char buffer[STRING_SIZE];

	if (irc_format_msg(msg, IRC_FMT_DEFAULT, buffer, STRING_SIZE) < 0)
		return(-1);
	fe_print(msg->server->status->window, buffer);
	return(0);
}


