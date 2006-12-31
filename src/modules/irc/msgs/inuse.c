/*
 * Module Name:		inuse.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Error Nickname In Use Message
 */

#include CONFIG_H
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

int irc_msg_inuse(char *env, struct irc_msg *msg)
{
	int i;
	char buffer[STRING_SIZE];

	if (msg->server && !(msg->server->bitflags & IRC_SBF_CONNECTED)) {
		for (i = 0;(i < IRC_MAX_NICK) && (msg->server->nick[i] != '\0');i++) ;
		if (i < IRC_MAX_NICK - 1) {
			msg->server->nick[i] = '_';
			msg->server->nick[i + 1] = '\0';
		}
		irc_change_nick(msg->server, msg->server->nick);
	}

	if (irc_format_msg(msg, IRC_FMT_NICKNAMEINUSE, buffer, STRING_SIZE) < 0)
		return(-1);
	fe_print(msg->server->status->window, buffer);
	return(0);
}


