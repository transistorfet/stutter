/*
 * Module Name:		dispatch.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		IRC Message Receiver/Dispatcher
 */

#include CONFIG_H
#include <stutter/modules/irc.h>

int irc_dispatch_msg(char *env, struct irc_msg *msg)
{
	switch (msg->cmd) {
		case 001:
		case 002:
		case 003:
		case 004:
			msg->server->bitflags |= IRC_SBF_CONNECTED;
			return(irc_msg_default(env, msg));
		case IRC_MSG_NOTICE:
			return(irc_msg_notice(env, msg));
		case IRC_MSG_JOIN:
			return(irc_msg_join(env, msg));
		case IRC_MSG_PART:
			return(irc_msg_part(env, msg));
		case IRC_MSG_PRIVMSG:
			return(irc_msg_privmsg(env, msg));
		case IRC_RPL_NAMREPLY:
			return(irc_msg_names(env, msg));
		case IRC_RPL_TOPIC:
		case IRC_MSG_TOPIC:
			return(irc_msg_topic(env, msg));
		case IRC_MSG_KICK:
			return(irc_msg_kick(env, msg));
		case IRC_MSG_NICK:
			return(irc_msg_nick(env, msg));
		case IRC_MSG_QUIT:
			return(irc_msg_quit(env, msg));
		default:
			return(irc_msg_default(env, msg));
	}

	return(0);
}


