/*
 * Command Name:	msg.c
 * Description:		Private Message Command
 */

#include <stutter/utils.h>
#include <stutter/modules/irc/irc.h>

int irc_cmd_msg(char *env, char *args)
{
	char *nick;
	int pos = 0;
	struct irc_server *server;

	if (!(server = irc_current_server()))
		return(-1);
	nick = util_get_arg(args, &pos);
	if ((*nick == '\0') || (args[pos] == '\0'))
		return(-1);
	irc_private_msg(server, nick, &args[pos]);
	return(0);
}


