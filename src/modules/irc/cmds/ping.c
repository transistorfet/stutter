/*
 * Command Name:	ping.c
 * Description:		CTCP Ping Command
 */

#include <time.h>
#include <stdio.h>

#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/modules/irc.h>

int irc_cmd_ping(char *env, char *args)
{
	char *target;
	char ping[50];
	struct irc_channel *channel;

	get_param_m(args, target, ' ');
	if (*target == '\0' || !(channel = irc_current_channel()))
		return(-1);
	if (sprintf(ping, "%lu", time(NULL)) < 0)
		return(-1);
	irc_ctcp_msg(channel->server, "PING", target, ping);
	return(0);
}


