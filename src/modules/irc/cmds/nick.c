/*
 * Command Name:	nick.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		Nick Change Command
 */

#include <stutter/modules/irc.h>

int irc_cmd_nick(char *env, char *args)
{
	struct irc_server *server;

	if (*args == '\0' || !(server = irc_current_server()))
		return(-1);
	return(irc_change_nick(server, args));
}


