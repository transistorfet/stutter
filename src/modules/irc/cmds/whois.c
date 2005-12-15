/*
 * Command Name:	whois.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Whois Command
 */

#include <frontend.h>
#include <modules/irc.h>

int irc_cmd_whois(char *env, char *args)
{
	struct irc_msg *msg;
	struct irc_server *server;

	if ((*args == '\0') || !(server = irc_current_server()))
		return(-1);
	if (!(msg = irc_create_msg(IRC_MSG_WHOIS, NULL, NULL, 1, args)))
		return(-1);
	irc_send_msg(server, msg);
	irc_destroy_msg(msg);
	return(0);
}


