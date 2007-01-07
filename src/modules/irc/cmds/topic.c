/*
 * Command Name:	topic.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Topic Command
 */

#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_topic(char *env, char *args)
{
	char *name;
	struct irc_msg *msg;
	struct irc_channel *channel;

	if ((*args == '\0') || !(channel = irc_current_channel()))
		return(-1);
	get_param_m(args, name, ' ');
	if (*args == '\0')
		msg = irc_create_msg(IRC_MSG_TOPIC, NULL, NULL, 1, name);
	else
		msg = irc_create_msg(IRC_MSG_TOPIC, NULL, NULL, 2, name, (!strcmp_icase(args, "none")) ? "" : args);
	if (!msg)
		return(-1);
	irc_send_msg(channel->server, msg);
	return(0);
}


