/*
 * Command Name:	notice.c
 * Description:		Notice Command
 */

#include <stutter/utils.h>
#include <stutter/modules/irc/irc.h>

int irc_cmd_notice(char *env, char *args)
{
	char *nick;
	int pos = 0;
	struct irc_server *server;

	if (*args == '\0' || !(server = irc_current_server()))
		return(-1);
	nick = util_get_arg(args, &pos);
	if (*nick == '\0')
		return(-1);
	irc_notice(server, nick, &args[pos]);
	return(0);
}


