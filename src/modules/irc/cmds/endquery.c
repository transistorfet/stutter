/*
 * Command Name:	endquery.c
 * Description:		End Query Command
 */

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/modules/irc/irc.h>

int irc_cmd_endquery(char *env, char *args)
{
	char *name;
	struct irc_server *server;
	struct irc_channel *channel;

	if (!(server = irc_current_server()))
		return(-1);

	name = util_get_arg(args, NULL);
	if (*name != '\0') {
		if (!(channel = irc_find_channel(&server->channels, name)))
			return(-1);
	}
	else if (!(channel = irc_current_channel()))
		return(-1);

	if ((channel->name[0] == '#') || (channel->name[0] == '&') || (channel->name[0] == '+') || (channel->name[0] == '!'))
		return(-1);
	irc_remove_channel(&server->channels, channel->name);
	return(0);
}


