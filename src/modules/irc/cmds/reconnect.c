/*
 * Command Name:	reconnect.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Reconnect to Server Command
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_reconnect(char *env, char *args)
{
	struct irc_server *server;

	if (!(server = irc_current_server()))
		return(-1);
	if (!irc_server_reconnect(server))
		return(-1);
	return(0);
}


