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
			msg->server->bitflags &= ~IRC_SBF_RECONNECTING;
			return(irc_msg_default(NULL, msg));
		case IRC_RPL_WHOISUSER:
			return(irc_msg_current(IRC_FMT_WHOISUSER, msg));
		case IRC_RPL_WHOISSERVER:
			return(irc_msg_current(IRC_FMT_WHOISSERVER, msg));
		case IRC_RPL_WHOISOPERATOR:
			return(irc_msg_current(IRC_FMT_WHOISOPERATOR, msg));
		case IRC_RPL_WHOISIDLE:
			return(irc_msg_current(IRC_FMT_WHOISIDLE, msg));
		case IRC_RPL_WHOISCHANNELS:
			return(irc_msg_current(IRC_FMT_WHOISCHANNELS, msg));
		case IRC_RPL_WHOISSPECIAL:
			return(irc_msg_current(IRC_FMT_WHOISSPECIAL, msg));
		case IRC_RPL_ENDOFWHOIS:
			return(0);
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
			return(irc_msg_default(NULL, msg));
	}

	return(0);
}


