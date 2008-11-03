/*
 * Command Name:	query.c
 * Description:		Query Channel Command
 */

#include <stdio.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/output.h>
#include <stutter/modules/irc/irc.h>

int irc_cmd_query(char *env, char *args)
{
	char *name;
	struct irc_server *server;
	struct irc_channel *channel;

	name = util_get_arg(args, NULL);
	if ((*name == '\0') || !(server = irc_current_server()))
		return(-1);
	if (name[0] == '#' || name[0] == '&' || name[0] == '+' || name[0] == '!')
		return(-1);

	if ((channel = irc_find_channel(&server->channels, name)))
		;
		// TODO how will you do this?
		//fe_show_widget(channel->window);
	//else if ((frame = fe_get_target(NULL, "frame"))
	//    && (window = fe_create_widget("irc", "text", name, frame))
	//    && (channel = irc_add_channel(&server->channels, name, window, server)))
	//	fe_show_widget(window);

	//else if (!)
	// TODO add channel signal (what should it be called)?
	// TODO emit create.output signal to notify for the new signal
	// TODO add a channel struct entry

/*
	else {
		OUTPUT_ERROR(IRC_ERR_QUERY_ERROR, name);
		return(-1);
	}
*/
	return(0);
}


