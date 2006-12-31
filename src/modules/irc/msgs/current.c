/*
 * Module Name:		current.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Generic Print-to-Current-Window Notification Message
 */

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_msg_current(char *env, struct irc_msg *msg)
{
	char buffer[LARGE_STRING_SIZE];
	struct irc_channel *channel;

	if (!(channel = irc_current_channel()))
		return(-1);
	if (irc_format_msg(msg, env ? env : IRC_FMT_CURRENT, buffer, LARGE_STRING_SIZE) < 0)
		return(-1);
	fe_print(channel->window, buffer);
	return(0);
}


