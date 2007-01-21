/*
 * Command Name:	kick.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Kick Command
 */

#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_kick(char *env, char *args)
{
	struct irc_msg *msg;
	char *channel, *nick;
	struct irc_server *server;

	if ((*args == '\0') || !(server = irc_current_server()))
		return(-1);
	get_param_m(args, channel, ' ');
	get_param_m(args, nick, ' ');
	if (*args == '\0')
		msg = irc_create_msg(IRC_MSG_KICK, NULL, NULL, 2, 0, channel, nick);
	else
		msg = irc_create_msg(IRC_MSG_KICK, NULL, NULL, 3, 0, channel, nick, args);
	if (!msg)
		return(-1);
	irc_send_msg(server, msg);
	return(0);
}


