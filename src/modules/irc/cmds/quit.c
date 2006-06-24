/*
 * Command Name:	quit.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Quit Command
 */

#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_cmd_quit(char *env, char *args)
{
	struct irc_msg *msg;

	// TODO remove this command and make this broadcast message a signal handler
	if (!(msg = irc_create_msg(IRC_MSG_QUIT, NULL, NULL, 1, (*args != '\0') ? args : IRC_QUIT_MSG)))
		return(-1);
	irc_broadcast_msg(msg);
	irc_destroy_msg(msg);
	fe_quit(0);
	return(0);
}


