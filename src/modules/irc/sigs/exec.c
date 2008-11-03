/*
 * Handler Name:	exec.c
 * Description:		Sends the output of the exec command to the current channel
 */

#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

int irc_sig_exec(void *ptr, struct signal_s *signal, va_list va)
{
	const char *text;
	struct irc_channel *channel;

	// TODO check that this is an exec signal
	text = va_arg(va, const char *);
	if (*text == '\0' || !(channel = irc_current_channel()))
		return(-1);
	irc_private_msg(channel->server, channel->name, text);
	return(0);
}


