/*
 * Module Name:		unknown.c
 * Description:		Unknown CTCP Message
 */

#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/variable.h>
#include <stutter/modules/irc/irc.h>

int irc_msg_ctcp_unknown(char *env, void *index, struct irc_msg *msg)
{
	IRC_MSG_CTCP_UNKNOWN_OUTPUT_JOINPOINT(msg, IRC_FMT_CTCP_UNKNOWN)
	return(0);
}


