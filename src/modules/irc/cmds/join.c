/*
 * Command Name:	join.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Join Channel Command
 */

#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/modules/irc.h>

int irc_cmd_join(char *env, char *args)
{
	char *name;
	struct irc_server *server;
	struct irc_msg *msg = NULL;
	struct irc_channel *channel;

	get_param_m(args, name, ' ');
	if ((*name == '\0') || !(server = irc_current_server()))
		return(-1);
	if (channel = irc_find_channel(server->channels, name))
		fe_select_widget("text", NULL, channel->window);
	else
		irc_join_channel(server, name);
	return(0);
}


