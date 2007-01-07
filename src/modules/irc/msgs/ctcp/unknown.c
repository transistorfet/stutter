/*
 * Module Name:		unknown.c
 * Version:		0.1
 * Module Requirements:	frontend ; variable ; modirc
 * Description:		Unknown CTCP Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/variable.h>
#include <stutter/modules/irc.h>

int irc_msg_ctcp_unknown(char *env, void *index, struct irc_msg *msg)
{
	int i;
	char buffer[STRING_SIZE];
	struct irc_channel *channel;

	if (!(channel = irc_current_channel()))
		return(-1);
	if (irc_format_msg(msg, IRC_FMT_CTCP_UNKNOWN, buffer, STRING_SIZE) < 0)
		return(-1);
	fe_print(channel->window, buffer);
	return(0);
}


