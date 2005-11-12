/*
 * Command Name:	names.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Names Command
 */

#include <frontend.h>
#include <modules/irc.h>

int irc_cmd_names(char *env, char *args)
{
	struct irc_msg *msg;
	struct irc_channel *channel;

	if (!(channel = irc_current_channel()))
		return(-1);
	if (!(msg = irc_create_msg(IRC_MSG_NAMES, NULL, NULL, 1, (*args != '\0') ? args : channel->name)))
		return(-1);
	irc_send_msg(channel->server, msg);
	irc_destroy_msg(msg);
	return(0);
}


