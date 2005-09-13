/*
 * Command Name:	notice.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		Notice Command
 */

#include <macros.h>
#include <modules/irc.h>

int irc_cmd_notice(char *env, char *str)
{
	char *nick;
	struct irc_server *server;

	if (*str == '\0' || !(server = irc_current_server()))
		return(-1);
	get_param_m(str, nick, ' ');
	irc_notice(server, nick, str);
	return(0);
}


