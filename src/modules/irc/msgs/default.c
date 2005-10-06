/*
 * Module Name:		default.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Default Notification Message (for Unhandled Messages)
 */

#include <frontend.h>
#include <modules/irc.h>

int irc_msg_default(char *env, struct irc_msg *msg)
{
	char *str;
	struct irc_server *server;

	if (!(server = irc_current_server()))
		return(-1);
	if (!(str = irc_format_msg(msg, IRC_FMT_DEFAULT)))
		return(-1);
	fe_print(server->status->window, str);
	return(0);
}


