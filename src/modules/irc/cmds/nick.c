/*
 * Command Name:	nick.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		Nick Change Command
 */

#include <modules/irc.h>

int irc_cmd_nick(char *env, char *str)
{
	struct irc_server *server;

	if (*str == '\0' || !(server = irc_current_server()))
		return(-1);
	return(irc_change_nick(server, str));
}


