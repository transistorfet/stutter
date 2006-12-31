/*
 * Command Name:	server.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Connect to Server Command
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/modules/irc.h>

int irc_cmd_server(char *env, char *args)
{
	int portnum;
	void *window;
	char *address, *port;
	struct irc_server *server;

	if ((*args == '\0'))
		return(-1);
	get_param_m(args, address, ' ');
	get_param_m(args, port, ' ');
	portnum = (*port == '\0') ? IRC_DEFAULT_PORT : atoi(port);

	if (server = irc_find_server(address))
		fe_select_widget("text", NULL, server->status->window);
	else if (window = fe_create_widget("irc", "text", NULL, NULL)) {
		if (!irc_server_connect(address, portnum, IRC_DEFAULT_NICK, window)) {
			fe_destroy_widget(window);
			return(-1);
		}
		fe_select_widget("text", NULL, window);
	}
	return(0);
}


