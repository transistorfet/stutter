/*
 * Command Name:	server.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Connect to Server Command
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_server(char *env, char *args)
{
	int portnum;
	void *window, *frame;
	struct irc_server *server;
	char *address, *port, *nick;

	if ((*args == '\0'))
		return(-1);
	get_param_m(args, address, ' ');
	get_param_m(args, port, ' ');
	portnum = (*port == '\0') ? IRC_DEFAULT_PORT : atoi(port);
	get_param_m(args, nick, ' ');
	if (*nick == '\0')
		nick = IRC_DEFAULT_NICK;

	if ((server = irc_find_server(address)))
		fe_show_widget(server->status->window);
	else if ((frame = fe_get_target(NULL, "frame")) && (window = fe_create_widget("irc", "text", address, frame))) {
		if (!(server = irc_create_server(nick, window))) {
			fe_destroy_widget(window);
			return(-1);
		}
		else if (irc_server_connect(server, address, portnum)) {
			irc_destroy_server(server);
			return(-1);
		}
		fe_show_widget(window);
	}
	return(0);
}


