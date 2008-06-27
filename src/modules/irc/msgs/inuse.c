/*
 * Module Name:		inuse.c
 * Description:		Error Nickname In Use Message
 */

#include CONFIG_H
#include <stutter/modules/irc.h>

int irc_msg_inuse(char *env, struct irc_msg *msg)
{
	string_t tmp;

	if (msg->server && !(msg->server->bitflags & IRC_SBF_CONNECTED) && msg->server->nick) {
		tmp = msg->server->nick;
		msg->server->nick = create_string("%s_", tmp);
		destroy_string(tmp);
		irc_change_nick(msg->server, msg->server->nick);
	}

	IRC_MSG_INUSE_OUTPUT_JOINPOINT(msg, IRC_FMT_NICKNAMEINUSE)
	return(0);
}


