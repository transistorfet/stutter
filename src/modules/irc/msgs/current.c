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
	struct irc_channel *channel;

	if (!(channel = irc_current_channel()))
		return(-1);
	IRC_MSG_CURRENT_OUTPUT_JOINPOINT(channel, msg, env ? env : IRC_FMT_CURRENT)
	return(0);
}


