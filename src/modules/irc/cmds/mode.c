/*
 * Command Name:	mode.c
 * Description:		Mode Command
 */

#include <stutter/utils.h>
#include <stutter/modules/irc/irc.h>

int irc_cmd_mode(char *env, char *args)
{
	int pos = 0;
	char *channel;
	struct irc_msg *msg;
	struct irc_server *server;

	if (!(server = irc_current_server()))
		return(-1);
	channel = util_get_arg(args, &pos);
	if ((*channel == '\0') || (args[pos] == '\0'))
		return(-1);
	if ((msg = irc_create_msg(IRC_MSG_MODE, NULL, NULL, 2, 0, channel, &args[pos])))
		irc_send_msg(server, msg);
	return(0);
}


