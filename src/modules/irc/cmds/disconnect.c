/*
 * Command Name:	disconnect.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Server Disconnect Command
 */

#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_disconnect(char *env, char *args)
{
	struct irc_msg *msg;
	struct irc_server *server;

	if (!(server = irc_current_server()))
		return(-1);
	if (!(msg = irc_create_msg(IRC_MSG_QUIT, NULL, NULL, 1, 0, (*args != '\0') ? args : IRC_QUIT_MSG)))
		return(-1);
	irc_send_msg(server, msg);
	irc_server_disconnect(server);
	return(0);
}


