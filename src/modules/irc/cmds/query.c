/*
 * Command Name:	query.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Query Channel Command
 */

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/modules/irc.h>

int irc_cmd_query(char *env, char *args)
{
	char *name;
	void *window;
	struct irc_server *server;
	struct irc_channel *channel;

	get_param_m(args, name, ' ');
	if ((*name == '\0') || !(server = irc_current_server()))
		return(-1);
	if (name[0] == '#' || name[0] == '&' || name[0] == '+' || name[0] == '!')
		return(-1);

	if (channel = irc_find_channel(server->channels, name))
		fe_select_widget("window", NULL, channel->window);
	else if ((window = fe_create_widget("irc", "window", NULL, NULL)) && (channel = irc_add_channel(server->channels, name, window, server)))
		fe_select_widget("window", NULL, window);
	else {
		if (window)
			fe_destroy_widget(window);
		IRC_ERROR_JOINPOINT(IRC_ERR_QUERY_ERROR, name)
		return(-1);
	}
	return(0);
}


