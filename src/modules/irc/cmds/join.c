/*
 * Command Name:	join.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Join Channel Command
 */

#include <frontend.h>
#include <modules/irc.h>

int irc_cmd_join(char *env, char *args)
{
	void *window;
	struct irc_server *server;
	struct irc_channel *channel;

	if ((*args == '\0') || !(server = irc_current_server()))
		return(-1);
	if (channel = irc_find_channel(server->channels, args))
		fe_set_current_widget(channel->window);
	else if (window = fe_create_widget("irc:window", fe_get_parent(fe_current_widget()))) {
		if (!irc_join_channel(server, args, window)) {
			fe_destroy_widget(window);
			return(-1);
		}
		fe_set_current_widget(window);
	}
	return(0);
}


