/*
 * Command Name:	whowas.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Whowas Command
 */

#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_whowas(char *env, char *args)
{
	struct irc_msg *msg;
	struct irc_server *server;

	if ((*args == '\0') || !(server = irc_current_server()))
		return(-1);
	if (!(msg = irc_create_msg(IRC_MSG_WHOWAS, NULL, NULL, 1, 0, args)))
		return(-1);
	irc_send_msg(server, msg);
	return(0);
}


