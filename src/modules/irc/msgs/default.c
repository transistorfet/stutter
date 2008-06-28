/*
 * Module Name:		default.c
 * Description:		Default Notification Message (for Unhandled Messages)
 */

#include CONFIG_H
#include <stutter/modules/irc/irc.h>

int irc_msg_default(char *env, struct irc_msg *msg)
{
	IRC_MSG_DEFAULT_OUTPUT_JOINPOINT(msg, env ? env : IRC_FMT_DEFAULT)
	return(0);
}


