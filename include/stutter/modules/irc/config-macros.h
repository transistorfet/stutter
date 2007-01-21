/*
 * Header Name:	config-macros.h
 * Description:	IRC Configuration Macros Header
 */

#ifndef _MODULES_IRC_CONFIG_MACROS_H
#define _MODULES_IRC_CONFIG_MACROS_H

#include <stutter/utils.h>
#include <stutter/signal.h>
#include <stutter/frontend.h>

#define IRC_PRINT_TO_CHANNEL(channel, msg, fmt) {			\
	char buffer[LARGE_STRING_SIZE];					\
	if (irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0)	\
		fe_print(channel->window, buffer);			\
}

#define IRC_PRINT_TO_STATUS(server, msg, fmt) {				\
	char buffer[LARGE_STRING_SIZE];					\
	if (irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0)	\
		fe_print(server->status->window, buffer);		\
}

#define IRC_PRINT_TO_CURRENT(msg, fmt) {				\
	struct irc_channel *channel;					\
	char buffer[LARGE_STRING_SIZE];					\
	if ((channel = irc_current_channel()) && irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0)	\
		fe_print(channel->window, buffer);			\
}

#define IRC_PRINT_TO_CURRENT_AND_STATUS(msg, fmt) {			\
	struct irc_channel *channel;					\
	char buffer[LARGE_STRING_SIZE];					\
	if ((channel = irc_current_channel()) && irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0) {	\
		if (channel != msg->server->status)			\
			fe_print(msg->server->status->window, buffer);	\
		fe_print(channel->window, buffer);			\
	}								\
}

#endif


