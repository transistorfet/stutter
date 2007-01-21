/*
 * Command Name:	ctcp.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		CTCP Message Command
 */

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/modules/irc.h>

int irc_cmd_ctcp(char *env, char *args)
{
	int i;
	char *nick, *cmd;
	struct irc_server *server;

	if (*args == '\0' || !(server = irc_current_server()))
		return(-1);
	get_param_m(args, nick, ' ');
	get_param_m(args, cmd, ' ');
	for (i = 0;cmd[i] != '\0';i++) {
		if ((cmd[i] >= 'a') && (cmd[i] <= 'z'))
			cmd[i] -= 0x20;
	}
	irc_ctcp_msg(server, cmd, nick, args);
	return(0);
}


