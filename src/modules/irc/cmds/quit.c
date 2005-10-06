/*
 * Command Name:	quit.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Quit Command
 */

#include <frontend.h>
#include <modules/irc.h>

int irc_cmd_quit(char *env, char *str)
{
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_QUIT, NULL, NULL, 1, (*str != '\0') ? str : IRC_QUIT_MSG)))
		return(-1);
	irc_broadcast_msg(msg);
	irc_destroy_msg(msg);
	fe_terminate();
	return(0);
}


