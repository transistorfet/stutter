/*
 * Module Name:		inuse.c
 * Description:		Error Nickname In Use Message
 */

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

int irc_msg_inuse(char *env, struct irc_msg *msg)
{
	string_t tmp;
	char buffer[LARGE_STRING_SIZE];

	if (!msg->server)
		return(-1);
	if (!(msg->server->bitflags & IRC_SBF_CONNECTED) && msg->server->nick) {
		tmp = msg->server->nick;
		msg->server->nick = create_string("%s_", tmp);
		destroy_string(tmp);
		irc_change_nick(msg->server, msg->server->nick);
	}
	if (irc_format_msg(msg, IRC_FMT_NICKNAMEINUSE, buffer, LARGE_STRING_SIZE) >= 0)
		signal_emit(msg->server->status->signal, buffer);
	return(0);
}


