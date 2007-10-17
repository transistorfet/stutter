/*
 * Header Name:	advice.h
 * Description:	IRC Advice Macros Header
 */

#ifndef _MODULES_IRC_ADVICE_H
#define _MODULES_IRC_ADVICE_H

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/signal.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

static inline void IRC_PRINT_TO_CHANNEL(struct irc_channel *channel, struct irc_msg *msg, char *fmt) {
	char buffer[LARGE_STRING_SIZE];
	if (irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0)
		fe_print(channel->window, buffer);
}

static inline void IRC_PRINT_TO_STATUS(struct irc_server *server, struct irc_msg *msg, char *fmt) {
	char buffer[LARGE_STRING_SIZE];
	if (irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0)
		fe_print(server->status->window, buffer);
}

static inline void IRC_PRINT_TO_CURRENT(struct irc_msg *msg, char *fmt) {
	struct irc_channel *channel;
	char buffer[LARGE_STRING_SIZE];
	if ((channel = irc_current_channel()) && irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0)
		fe_print(channel->window, buffer);
}

static inline void IRC_PRINT_TO_CURRENT_AND_STATUS(struct irc_msg *msg, char *fmt) {
	struct irc_channel *channel;
	char buffer[LARGE_STRING_SIZE];
	if ((channel = irc_current_channel()) && irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0) {
		if (channel != msg->server->status)
			fe_print(msg->server->status->window, buffer);
		fe_print(channel->window, buffer);
	}
}

static inline void IRC_PRINT_TO_SERVER(struct irc_server *server, struct irc_msg *msg, char *fmt) {
	struct irc_channel *channel;
	char buffer[LARGE_STRING_SIZE];
	if (irc_format_msg(msg, fmt, buffer, LARGE_STRING_SIZE) >= 0) {
		channel = irc_channel_get_first(server->channels);
		while (channel) {
			fe_print(channel->window, buffer);
			channel = irc_channel_get_next(channel);
		}
	}
}


#endif


