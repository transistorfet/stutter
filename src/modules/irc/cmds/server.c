/*
 * Command Name:	server.c
 * Description:		Connect to Server Command
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/modules/irc/irc.h>

int irc_cmd_server(char *env, char *args)
{
	int portnum, pos = 0;
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
		;
		// TODO how do you do this functionality?
		//fe_show_widget(server->status->window);
	else {
		if (!(server = IRC_SERVER(create_object(OBJECT_TYPE_S(&irc_server_type), "s", nick))))
			return(-1);
		else if (irc_server_connect(server, address, portnum)) {
			destroy_object(OBJECT_S(server));
			return(-1);
		}
		// TODO howe do you do this?
		//fe_show_widget(window);
	}
	return(0);
}


