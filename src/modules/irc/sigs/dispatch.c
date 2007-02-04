/*
 * Module Name:		dispatch.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		IRC Message Receiver/Dispatcher
 */

#include <string.h>

#include CONFIG_H
#include <stutter/modules/irc.h>

int irc_sig_dispatch_msg(char *env, int cmd, struct irc_msg *msg)
{
	switch (msg->cmd) {
		case 001:
			/** Some servers have a limit to the nick length so
			    make sure we are using the right nick */
			if (strcmp(msg->server->nick, msg->params[0])) {
				strncpy(msg->server->nick, msg->params[0], IRC_MAX_NICK - 1);
				msg->server->nick[IRC_MAX_NICK - 1] = '\0';
			}
			msg->server->bitflags |= IRC_SBF_CONNECTED;
		case 002:
		case 003:
		case 004:
		case 005:
		case IRC_RPL_LUSERCLIENT:
		case IRC_RPL_LUSEROP:
		case IRC_RPL_LUSERUNKNOWN:
		case IRC_RPL_LUSERCHANNELS:
		case IRC_RPL_LUSERME:
		case IRC_RPL_MOTDSTART:
		case IRC_RPL_MOTD:
		case IRC_RPL_ENDOFMOTD:
			return(irc_msg_default(IRC_FMT_STATUS, msg));
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
		case IRC_RPL_WHOWASUSER:
			return(irc_msg_current(IRC_FMT_WHOWASUSER, msg));
		case IRC_RPL_ENDOFWHOWAS:
			return(0);
		case IRC_RPL_ENDOFNAMES:
			return(0);
		case IRC_ERR_NOSUCHNICK:
			return(irc_msg_current(IRC_FMT_NOSUCHNICK, msg));
		case IRC_ERR_NICKNAMEINUSE:
			return(irc_msg_inuse(env, msg));
		case IRC_MSG_MODE:
			return(irc_msg_mode(env, msg));
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


