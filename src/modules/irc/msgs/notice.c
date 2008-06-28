/*
 * Module Name:		notice.c
 * Description:		Notice Notification Message
 */

#include CONFIG_H
#include <stutter/string.h>
#include <stutter/modules/irc.h>

/**
 * Prints message to current channel and status channel.
 */
int irc_msg_notice(char *env, struct irc_msg *msg)
{
	if (msg->num_ctcps)
		irc_dispatch_ctcp_msg(msg);
	else {
		IRC_MSG_NOTICE_OUTPUT_JOINPOINT(msg, msg->nick ? IRC_FMT_NOTICE : IRC_FMT_NOTICE_SELF)
	}
	return(0);
}


