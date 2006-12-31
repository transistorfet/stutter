/*
 * Module Name:		format.c
 * Version:		0.1
 * Module Requirements:	utils ; string ; modirc ; modbase
 * Description:		IRC Message Formatter
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/lib/string.h>
#include <stutter/lib/macros.h>
#include <stutter/modules/irc.h>
#include <stutter/modules/base.h>
#include <stutter/frontend/surface.h>

#define msg_copy_str_m(buffer, str, count, max)				\
	if (str) {							\
		strncpy(&buffer[count], str, (max - count - 1));	\
		count += strlen(str);					\
	}

static int irc_format_parse_text(char *, char *, int);

/**
 * Format the data in the given message (and any variable parameters) using
 * the given format string into the given buffer up to the given maximum
 * number of characters.  If successful, the number of characters written
 * to the buffer is returned; otherwise a negative number is returned.
 */
int irc_format_msg(struct irc_msg *msg, char *fmt, char *buffer, int max)
{
	int i, k;
	int j = 0;
	char *tmp;
	time_t current_time;
	struct tm *timestamp;
	char value[STRING_SIZE];
	struct irc_server *server;
	struct irc_channel *channel;

	max--;
	channel = irc_current_channel();
	server = channel ? channel->server : NULL;
	for (i = 0;(fmt[i] != '\0') && (j < max);i++) {
		if (fmt[i] == '%') {
			switch (fmt[++i]) {
				case '@':
					timestamp = localtime(&msg->time);
					j += strftime(&buffer[j], max - j, IRC_TIMESTAMP, timestamp);
					break;
				case 'B':
					#ifdef IRC_BANNER
					msg_copy_str_m(buffer, IRC_BANNER, j, max);
					#endif
					break;
				case 'n':
					if (server)
						msg_copy_str_m(buffer, server->nick, j, max);
					break;
				case 'c':
					if (channel)
						msg_copy_str_m(buffer, channel->name, j, max);
					break;
				case 't':
					if (channel)
						msg_copy_str_m(buffer, channel->topic, j, max);
					break;
				case 'C':
					if (tmp = irc_command_name(msg->cmd))
						msg_copy_str_m(buffer, tmp, j, max);
					break;
				case 'N':
					if (msg)
						msg_copy_str_m(buffer, msg->nick, j, max);
					break;
				case 'H':
					if (msg)
						msg_copy_str_m(buffer, msg->host, j, max);
					break;
				case 'M':
					if (msg && msg->text)
						j += irc_format_parse_text(msg->text, &buffer[j], max - j + 1);
					break;
				case 'P':
					if (msg) {
						k = fmt[++i] - 0x31;
						if ((k >= 0) && (k < msg->num_params))
							msg_copy_str_m(buffer, msg->params[k], j, max);
					}
					break;
				case '%':
					buffer[j++] = '%';
					break;
				default:
					break;
			}
		}
		else if (fmt[i] == '$') {
			if (util_expand_variable(&fmt[i], value, STRING_SIZE, &i) > 0)
				j += irc_format_msg(msg, value, &buffer[j], max - j + 1);
		}
		else
			buffer[j++] = fmt[i];
	}
	buffer[j] = '\0';
	return(j);
}

/*** Local Functions ***/

static int irc_format_parse_text(char *str, char *buffer, int max)
{
	attrib_t attrib;
	int i = 0, j = 0, write = 0;
	attrib_t def_attrib = { SA_METHOD_OR, SA_NORMAL, { SC_ENC_MAPPING, SC_MAP_CURRENT_COLOUR }, { SC_ENC_MAPPING, SC_MAP_CURRENT_COLOUR } };

	max--;
	attrib = def_attrib;
	while ((str[i] != '\0') && (j < max)) {
		switch (str[i]) {
			case 0x02: {
				attrib.method = SA_METHOD_TOGGLE;
				attrib.style ^= SA_BOLD;
				write = 1;
				i++;
				break;
			}
			case 0x03: {
				i++;
				if (!is_number_char_m(str[i])) {
					attrib.fg.colour = SC_MAP_DEFAULT_COLOUR;
					attrib.bg.colour = SC_MAP_DEFAULT_COLOUR;
				}
				else {
					attrib.fg.colour = str[i++] - 0x30;
					if (is_number_char_m(str[i]))
						attrib.fg.colour = (attrib.fg.colour * 10) + (str[i++] - 0x30);
					if ((str[i] == ',') && ++i && is_number_char_m(str[i])) {
						attrib.bg.colour = str[i++] - 0x30;
						if (is_number_char_m(str[i]))
							attrib.bg.colour = (attrib.bg.colour * 10) + (str[i++] - 0x30);
					}
				}
				write = 1;
				break;
			}
			case 0x0F: {
				attrib.method = SA_METHOD_SET;
				attrib.style = SA_NORMAL;
				write = 1;
				i++;
				break;
			}
			case 0x16: {
				attrib.method = SA_METHOD_TOGGLE;
				attrib.style ^= SA_INVERSE;
				write = 1;
				i++;
				break;
			}
			default: {
				if (write) {
					write = 0;
					if (max - j < sizeof(attrib_t) + 1)
						break;
					buffer[j++] = '\x01';
					j += util_encode_bytes((unsigned char *) &attrib, sizeof(attrib_t), &buffer[j], max - j + 1);
					attrib = def_attrib;
				}
				buffer[j++] = str[i++];
				break;
			}
		}
	}
	buffer[j] = '\0';
	return(j);
}


