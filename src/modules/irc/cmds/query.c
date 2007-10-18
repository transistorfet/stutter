/*
 * Command Name:	query.c
 * Description:		Query Channel Command
 */

#include <stdio.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_query(char *env, char *args)
{
	char *name;
	void *window, *frame;
	struct irc_server *server;
	struct irc_channel *channel;

	get_param_m(args, name, ' ');
	if ((*name == '\0') || !(server = irc_current_server()))
		return(-1);
	if (name[0] == '#' || name[0] == '&' || name[0] == '+' || name[0] == '!')
		return(-1);

	if ((channel = irc_find_channel(server->channels, name)))
		fe_show_widget(channel->window);
	else if ((frame = fe_get_target(NULL, "frame"))
	    && (window = fe_create_widget("irc", "text", name, frame))
	    && (channel = irc_add_channel(server->channels, name, window, server)))
		fe_show_widget(window);
	else {
		if (!channel && window)
			fe_destroy_widget(window);
		IRC_ERROR_JOINPOINT(IRC_ERR_QUERY_ERROR, name)
		return(-1);
	}
	return(0);
}


