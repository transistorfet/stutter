/*
 * Module Name:		notice.c
 * Version:		0.1
 * Module Requirements:	frontend ; string ; modirc
 * Description:		Notice Notification Message
 */

#include CONFIG_H
#include <stutter/string.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Prints message to current channel and status channel.
 */
int irc_msg_notice(char *env, struct irc_msg *msg)
{
	if (msg->num_ctcps)
		emit_signal(NULL, "irc.dispatch_ctcp", msg);
	else {
		IRC_MSG_NOTICE_OUTPUT_JOINPOINT(msg, msg->nick ? IRC_FMT_NOTICE : IRC_FMT_NOTICE_SELF)
	}
	return(0);
}


