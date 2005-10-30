/*
 * Command Name:	leave.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Leave Channel Command
 */

#include <frontend.h>
#include <modules/irc.h>

int irc_cmd_leave(char *env, char *args)
{
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(args)))
		return(-1);
	fe_destroy_widget(channel->window);
	irc_leave_channel(channel->server, args);
	return(0);
}


