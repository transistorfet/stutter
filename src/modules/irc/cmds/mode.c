/*
 * Command Name:	mode.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		Mode Command
 */

#include <stutter/macros.h>
#include <stutter/modules/irc.h>

int irc_cmd_mode(char *env, char *args)
{
	char *channel;
	struct irc_msg *msg;
	struct irc_server *server;

	if (*args == '\0' || !(server = irc_current_server()))
		return(-1);
	get_param_m(args, channel, ' ');
	if (*args == '\0')
		return(-1);
	if (msg = irc_create_msg(IRC_MSG_MODE, NULL, NULL, 2, 0, channel, args))
		irc_send_msg(server, msg);
	return(0);
}


