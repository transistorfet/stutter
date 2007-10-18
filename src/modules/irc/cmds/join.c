/*
 * Command Name:	join.c
 * Description:		Join Channel Command
 */

#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_join(char *env, char *args)
{
	char *name;
	struct irc_server *server;
	struct irc_channel *channel;

	get_param_m(args, name, ' ');
	if ((*name == '\0') || !(server = irc_current_server()))
		return(-1);
	if ((channel = irc_find_channel(server->channels, name)))
		fe_show_widget(channel->window);
	else
		irc_join_channel(server, name);
	return(0);
}


