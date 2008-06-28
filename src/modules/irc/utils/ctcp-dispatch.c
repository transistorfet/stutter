/*
 * Module Name:		ctcp-dispatch.c
 * Description:		IRC CTCP Message Receiver/Dispatcher
 */

#include <string.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/modules/irc.h>

int irc_dispatch_ctcp_msg(struct irc_msg *msg)
{
	if (!strcmp_icase(msg->ctcps[0].tag, "ACTION"))
		return(irc_msg_ctcp_action(env, msg));
	else if (!strcmp_icase(msg->ctcps[0].tag, "PING"))
		return(irc_msg_ctcp_ping(env, msg));
	else if (!strcmp_icase(msg->ctcps[0].tag, "VERSION"))
		return(irc_msg_ctcp_version(env, msg));
	return(0);
}


