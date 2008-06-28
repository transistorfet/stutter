/*
 * Handler Name:	exec.c
 * Description:		Sends the output of the exec command to the current channel
 */

#include <stutter/modules/irc/irc.h>

int irc_sig_exec(void *ptr, void *index, char *args)
{
	struct irc_channel *channel;

	if (*args == '\0' || !(channel = irc_current_channel()))
		return(-1);
	irc_private_msg(channel->server, channel->name, args);
	return(0);
}


