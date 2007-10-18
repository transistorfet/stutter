/*
 * Command Name:	msg.c
 * Description:		Private Message Command
 */

#include <stutter/macros.h>
#include <stutter/modules/irc.h>

int irc_cmd_msg(char *env, char *args)
{
	char *nick;
	struct irc_server *server;

	if (*args == '\0' || !(server = irc_current_server()))
		return(-1);
	get_param_m(args, nick, ' ');
	irc_private_msg(server, nick, args);
	return(0);
}


