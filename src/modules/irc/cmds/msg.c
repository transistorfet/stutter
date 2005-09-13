/*
 * Command Name:	msg.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		Private Message Command
 */

#include <macros.h>
#include <modules/irc.h>

int irc_cmd_msg(char *env, char *str)
{
	char *nick;
	struct irc_server *server;

	if (*str == '\0' || !(server = irc_current_server()))
		return(-1);
	get_param_m(str, nick, ' ');
	irc_private_msg(server, nick, str);
	return(0);
}


