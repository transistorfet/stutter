/*
 * Command Name:	notice.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		Notice Command
 */

#include <lib/macros.h>
#include <modules/irc.h>

int irc_cmd_notice(char *env, char *args)
{
	char *nick;
	struct irc_server *server;

	if (*args == '\0' || !(server = irc_current_server()))
		return(-1);
	get_param_m(args, nick, ' ');
	irc_notice(server, nick, args);
	return(0);
}


