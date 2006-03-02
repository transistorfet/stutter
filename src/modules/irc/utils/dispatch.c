/*
 * Module Name:		dispatch.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		IRC Message Receiver/Dispatcher
 */

#include CONFIG_H
#include <stutter/modules/irc.h>

int irc_dispatch_msg(struct irc_server *server, struct irc_msg *msg)
{
	switch (msg->cmd) {
		case IRC_MSG_NOTICE:
			return(irc_msg_notice(msg->server, msg));
		case IRC_MSG_JOIN:
			return(irc_msg_join(msg->server, msg));
		case IRC_MSG_PART:
			return(irc_msg_part(msg->server, msg));
		case IRC_MSG_PRIVMSG:
			return(irc_msg_privmsg(msg->server, msg));
		case IRC_RPL_NAMREPLY:
			return(irc_msg_names(msg->server, msg));
		case IRC_RPL_TOPIC:
		case IRC_MSG_TOPIC:
			return(irc_msg_topic(msg->server, msg));
		case IRC_MSG_KICK:
			return(irc_msg_kick(msg->server, msg));
		case IRC_MSG_NICK:
			return(irc_msg_nick(msg->server, msg));
		case IRC_MSG_QUIT:
			return(irc_msg_quit(msg->server, msg));
		default:
			return(irc_msg_default(msg->server, msg));
	}

	return(0);
}


