/*
 * Command Name:	topic.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Topic Command
 */

#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/modules/irc.h>

int irc_cmd_topic(char *env, char *args)
{
	char *name;
	struct irc_msg *msg;
	struct irc_channel *channel;

	get_param_m(args, name, ' ');
	if (!(channel = irc_current_channel()))
		return(-1);
	if (!(msg = irc_create_msg(IRC_MSG_TOPIC, NULL, NULL, 1, (*args != '\0') ? args : channel->name)))
		return(-1);
	irc_send_msg(channel->server, msg);
	return(0);
}


