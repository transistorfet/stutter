/*
 * Command Name:	disconnect.c
 * Description:		Server Disconnect Command
 */

#include CONFIG_H
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
	IRC_OUTPUT_JOINPOINT(IRC_SERVER_DISCONNECTED, server->address)
	return(0);
}


