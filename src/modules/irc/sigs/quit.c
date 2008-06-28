/*
 * Handler Name:	quit.c
 * Description:		Send a quit message when the program is terminated.
 */

#include CONFIG_H
#include <stutter/modules/irc/irc.h>

int irc_sig_quit(void *ptr, void *index, char *args)
{
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_QUIT, NULL, NULL, 1, 0, (*args != '\0') ? args : IRC_QUIT_MSG)))
		return(-1);
	irc_broadcast_msg(msg);
	return(0);
}


