/*
 * Module Name:		format.c
 * Version:		0.1
 * Module Requirements:	modirc
 * Description:		IRC Message Formatter
 */

#include <time.h>
#include <stdlib.h>
#include <stdarg.h>

#include CONFIG_H
#include <nit/string.h>
#include <modules/irc.h>

#define MAX_BUFFER		1024

#define msg_copy_str_m(buffer, str, count)				\
	if (str) {							\
		strncpy(&buffer[count], str, (MAX_BUFFER - j - 1));	\
		count += strlen(str);					\
	}

/**
 * Allocate a string and format the data in the given message (and any
 * variable parameters) using the given format string.  If successful,
 * a pointer to the malloc'd string is returned; otherwise NULL is returned
 */
char *irc_format_msg(struct irc_msg *msg, char *str, ...)
{
	char *tmp;
	va_list va;
	int i, j = 0, k;
	time_t current_time;
	struct tm *timestamp;
	char buffer[MAX_BUFFER];
	struct irc_server *server;
	struct irc_channel *channel;

	va_start(va, str);
	channel = irc_current_channel();
	if (channel)
		server = channel->server;
	for (i = 0;(str[i] != '\0') && (j < MAX_BUFFER - 1);i++) {
		if (str[i] == '%') {
			switch (str[++i]) {
				case '@':
					timestamp = localtime(&msg->time);
					j += strftime(&buffer[j], MAX_BUFFER - j, IRC_TIMESTAMP, timestamp);
					break;
				case 'H':
					current_time = time(NULL);
					timestamp = localtime(&current_time);
					j += strftime(&buffer[j], MAX_BUFFER - j, "%H:%M", timestamp);
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
					msg_copy_str_m(buffer, msg->nick, j);
					break;
				case 'S':
					msg_copy_str_m(buffer, msg->server, j);
					break;
				case 'M':
					msg_copy_str_m(buffer, msg->text, j);
					break;
				case 'P':
					k = str[++i] - 0x31;
					if ((k >= 0) && (k < msg->num_params))
						msg_copy_str_m(buffer, msg->params[k], j);
					break;
				case '%':
					buffer[j++] = '%';
					break;
				default:
					break;
			}
		}
		else
			buffer[j++] = str[i];
	}
	buffer[j++] = '\0';
	va_end(va);

	return(create_string(buffer));

//	if (!(ret = (char *) malloc(j)))
//		return(NULL);
//	strcpy(ret, buffer);
//	return(ret);
}


