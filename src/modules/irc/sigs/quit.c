/*
 * Handler Name:	quit.c
 * Description:		Send a quit message when the program is terminated.
 */

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

int irc_sig_quit(void *ptr, struct signal_s *signal, va_list va)
{
	const char *text;
	struct irc_msg *msg;

	// TODO check that this is a quit signal
	text = va_arg(va, const char *);
	if (!(msg = irc_create_msg(IRC_MSG_QUIT, NULL, NULL, 1, 0, (*text != '\0') ? text : IRC_QUIT_MSG)))
		return(-1);
	irc_broadcast_msg(msg);
	return(0);
}


