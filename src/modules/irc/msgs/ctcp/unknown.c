/*
 * Module Name:		unknown.c
 * Description:		Unknown CTCP Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/variable.h>
#include <stutter/modules/irc/irc.h>

int irc_msg_ctcp_unknown(char *env, void *index, struct irc_msg *msg)
{
	char buffer[LARGE_STRING_SIZE];

	// TODO fix this so that it prints to the current channel or maybe just the status or somethnig
//	if (irc_format_msg(msg, IRC_FMT_CTCP_UNKNOWN, buffer, LARGE_STRING_SIZE) >= 0)
//		signal_emit(channel->signal, buffer);
	return(0);
}


