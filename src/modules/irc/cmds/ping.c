/*
 * Command Name:	ping.c
 * Version:		0.1
 * Module Requirements:	string ; modirc
 * Description:		CTCP Ping Command
 */

#include <time.h>
#include <stdio.h>

#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>
#include <stutter/modules/irc.h>

int irc_cmd_ping(char *env, char *args)
{
	char *target;
	char ping[50];
	struct irc_channel *channel;

	get_param_m(args, target, ' ');
	if (*target == '\0' || !(channel = irc_current_channel()))
		return(-1);
	if (sprintf(ping, "\x01PING %d\x01", time(NULL)) < 0)
		return(-1);
	irc_private_msg(channel->server, target, ping);
	return(0);
}

