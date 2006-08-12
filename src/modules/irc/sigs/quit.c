/*
 * Handler Name:	quit.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Send a quit message when the program is terminated.
 */

#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_sig_quit(void *ptr, void *index, char *args)
{
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_QUIT, NULL, NULL, 1, (*args != '\0') ? args : IRC_QUIT_MSG)))
		return(-1);
	irc_broadcast_msg(msg);
	return(0);
}


