/*
 * Command Name:	server.c
 * Description:		Connect to Server Command
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_server(char *env, char *args)
{
	int portnum, pos = 0;
	void *window, *frame;
	struct irc_server *server;
	char *address, *port, *nick;

	if (*args == '\0')
		return(-1);
	address = util_get_arg(args, &pos);
	port = util_get_arg(&args[pos], &pos);
	portnum = (*port == '\0') ? IRC_DEFAULT_PORT : atoi(port);
	nick = util_get_arg(&args[pos], &pos);
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


