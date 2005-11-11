/*
 * Module Name:		default.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Default Notification Message (for Unhandled Messages)
 */

#include <frontend.h>
#include <modules/irc.h>

int irc_msg_default(struct irc_server *server, struct irc_msg *msg)
{
	char *str;

	if (!(str = irc_format_msg(msg, IRC_FMT_DEFAULT)))
		return(-1);
	fe_print(server->status->window, str);
	return(0);
}


