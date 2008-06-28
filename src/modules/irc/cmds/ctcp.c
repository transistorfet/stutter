/*
 * Command Name:	ctcp.c
 * Description:		CTCP Message Command
 */

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/modules/irc/irc.h>

int irc_cmd_ctcp(char *env, char *args)
{
	int pos = 0;
	char *nick, *cmd;
	struct irc_server *server;

	if (!(server = irc_current_server()))
		return(-1);
	nick = util_get_arg(args, &pos);
	cmd = util_get_arg(&args[pos], &pos);
	if ((*nick == '\0') || (*cmd == '\0'))
		return(-1);
	UPPERCASE(cmd);
	irc_ctcp_msg(server, cmd, nick, &args[pos]);
	return(0);
}


