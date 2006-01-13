/*
 * Command Name:	join.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Join Channel Command
 */

#include <frontend.h>
#include <lib/macros.h>
#include <modules/irc.h>

int irc_cmd_join(char *env, char *args)
{
	char *name;
	void *window;
	struct irc_server *server;
	struct irc_channel *channel;

	get_param_m(args, name, ' ');
	if ((*name == '\0') || !(server = irc_current_server()))
		return(-1);
	if (channel = irc_find_channel(server->channels, name))
		fe_select_widget("window", NULL, channel->window);
	else if (window = fe_create_widget("irc", "window", NULL, NULL)) {
		if (!irc_join_channel(server, name, window)) {
			fe_destroy_widget(window);
			return(-1);
		}
		fe_select_widget("window", NULL, window);
	}
	return(0);
}


