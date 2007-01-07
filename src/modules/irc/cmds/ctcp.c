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
	char buffer[STRING_SIZE];
	struct irc_server *server;

	if (*args == '\0' || !(server = irc_current_server()))
		return(-1);
	get_param_m(args, nick, ' ');
	if (snprintf(buffer, STRING_SIZE, "\x01%s\x01", args) < 0)
		return(-1);
	for (i = 0;buffer[i] != '\0' && buffer[i] != ' ';i++) {
		if ((buffer[i] >= 'a') && (buffer[i] <= 'z'))
			buffer[i] -= 0x20;
	}
	irc_private_msg(server, nick, buffer);
	return(0);
}


