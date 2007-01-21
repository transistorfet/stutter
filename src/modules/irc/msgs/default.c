/*
 * Module Name:		default.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Default Notification Message (for Unhandled Messages)
 */

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_msg_default(char *env, struct irc_msg *msg)
{
	IRC_MSG_DEFAULT_OUTPUT_JOINPOINT(msg, env ? env : IRC_FMT_DEFAULT)
	return(0);
}


