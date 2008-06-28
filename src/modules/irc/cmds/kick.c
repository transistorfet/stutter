/*
 * Command Name:	kick.c
 * Description:		Kick Command
 */

#include <stutter/utils.h>
#include <stutter/modules/irc/irc.h>

int irc_cmd_kick(char *env, char *args)
{
	int pos = 0;
	struct irc_msg *msg;
	char *channel, *nick;
	struct irc_server *server;

	if (!(server = irc_current_server()))
		return(-1);
	channel = util_get_arg(args, &pos);
	nick = util_get_arg(&args[pos], &pos);
	if ((*channel == '\0') || (*nick == '\0'))
		return(-1);
	if (args[pos] == '\0')
		msg = irc_create_msg(IRC_MSG_KICK, NULL, NULL, 2, 0, channel, nick);
	else
		msg = irc_create_msg(IRC_MSG_KICK, NULL, NULL, 3, 0, channel, nick, &args[pos]);
	if (!msg)
		return(-1);
	irc_send_msg(server, msg);
	return(0);
}


