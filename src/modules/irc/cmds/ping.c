/*
 * Command Name:	ping.c
 * Description:		CTCP Ping Command
 */

#include <time.h>
#include <stdio.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/string.h>
#include <stutter/modules/irc/irc.h>

int irc_cmd_ping(char *env, char *args)
{
	char *target;
	char ping[SMALL_STRING_SIZE];
	struct irc_channel *channel;

	target = util_get_arg(args, NULL);
	if (*target == '\0' || !(channel = irc_current_channel()))
		return(-1);
	if (snprintf(ping, SMALL_STRING_SIZE, "%lu", time(NULL)) < 0)
		return(-1);
	irc_ctcp_msg(channel->server, "PING", target, ping);
	return(0);
}


