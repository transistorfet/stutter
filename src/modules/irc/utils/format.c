/*
 * Module Name:		format.c
 * Version:		0.1
 * Module Requirements:	utils ; string ; modirc ; modbase
 * Description:		IRC Message Formatter
 */

#include <time.h>
#include <stdlib.h>

#include CONFIG_H
#include <utils.h>
#include <lib/string.h>
#include <modules/irc.h>
#include <modules/base.h>

#define MAX_BUFFER		1024

#define msg_copy_str_m(buffer, str, count)				\
	if (str) {							\
		strncpy(&buffer[count], str, (MAX_BUFFER - count - 1));	\
		count += strlen(str);					\
	}

static int irc_format_buffer(struct irc_msg *, char *, char *, int);

/**
 * Allocate a string and format the data in the given message (and any
 * variable parameters) using the given format string.  If successful,
 * a pointer to the malloc'd string is returned; otherwise NULL is returned
 */
string_t irc_format_msg(struct irc_msg *msg, char *str)
{
	int j = 0;
	char buffer[MAX_BUFFER];

	j += irc_format_buffer(msg, str, buffer, MAX_BUFFER - 1);
	buffer[j++] = '\0';

	return(create_string("%s", buffer));
}

/*** Local Functions ***/

static int irc_format_buffer(struct irc_msg *msg, char *str, char *buffer, int max)
{
	int i, k;
	int j = 0;
	char *tmp;
	string_t value;
	time_t current_time;
	struct tm *timestamp;
	struct irc_server *server;
	struct irc_channel *channel;

	channel = irc_current_channel();
	server = channel ? channel->server : NULL;
	for (i = 0;(str[i] != '\0') && (j < max);i++) {
		if (str[i] == '%') {
			switch (str[++i]) {
				case '@':
					timestamp = localtime(&msg->time);
					j += strftime(&buffer[j], MAX_BUFFER - j, IRC_TIMESTAMP, timestamp);
					break;
				case 'B':
					#ifdef IRC_BANNER
					msg_copy_str_m(buffer, IRC_BANNER, j);
					#endif
					break;
				case 'n':
					if (server)
						msg_copy_str_m(buffer, server->nick, j);
					break;
				case 'c':
					if (channel)
						msg_copy_str_m(buffer, channel->name, j);
					break;
				case 't':
					if (channel)
						msg_copy_str_m(buffer, channel->topic, j);
					break;
				case 'C':
					if (tmp = irc_command_name(msg->cmd))
						msg_copy_str_m(buffer, tmp, j);
					break;
				case 'N':
					if (msg)
						msg_copy_str_m(buffer, msg->nick, j);
					break;
				case 'S':
					if (msg)
						msg_copy_str_m(buffer, msg->host, j);
					break;
				case 'M':
					if (msg)
						msg_copy_str_m(buffer, msg->text, j);
					break;
				case 'P':
					if (msg) {
						k = str[++i] - 0x31;
						if ((k >= 0) && (k < msg->num_params))
							msg_copy_str_m(buffer, msg->params[k], j);
					}
					break;
				case '%':
					buffer[j++] = '%';
					break;
				default:
					break;
			}
		}
		else if (str[i] == '$') {
			if (value = util_expand_variable(&str[i], &i)) {
				j += irc_format_buffer(msg, value, &buffer[j], max - j);
				destroy_string(value);
			}
		}
		else
			buffer[j++] = str[i];
	}
	return(j);
}

