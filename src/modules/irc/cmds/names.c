/*
 * Command Name:	names.c
 * Description:		Names Command
 */

#include <stutter/utils.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_names(char *env, char *args)
{
	char *name;
	struct irc_msg *msg;
	struct irc_channel *channel;

	if (!(channel = irc_current_channel()))
		return(-1);
	name = util_get_arg(args, NULL);
	if (*name == '\0')
		name = channel->name;
	if (!(msg = irc_create_msg(IRC_MSG_NAMES, NULL, NULL, 1, 0, name)))
		return(-1);
	irc_send_msg(channel->server, msg);
	return(0);
}


