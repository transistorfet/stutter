/*
 * Module Name:		msg.c
 * Description:		Message Manager
 */


#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/memory.h>
#include <stutter/object.h>
#include <stutter/modules/irc/msg.h>

struct irc_msg_commands {
	char *name;
	short cmd;
	unsigned int use_text : 1;
	unsigned int min_params : 4;
	unsigned int max_params : 4;
};

static struct irc_msg_commands msg_commands[] = {
	{ "PASS", IRC_MSG_PASS, 0, 1, 1 },
	{ "NICK", IRC_MSG_NICK, 0, 1, 1 },
	{ "USER", IRC_MSG_USER, 1, 4, 4 },
	{ "OPER", IRC_MSG_OPER, 0, 2, 2 },
	{ "QUIT", IRC_MSG_QUIT, 1, 0, 1 },
	{ "JOIN", IRC_MSG_JOIN, 0, 1, 2 },
	{ "PART", IRC_MSG_PART, 1, 1, 2 },
	{ "MODE", IRC_MSG_MODE, 0, 2, 0 },
	{ "TOPIC", IRC_MSG_TOPIC, 1, 1, 2 },
	{ "NAMES", IRC_MSG_NAMES, 0, 1, 2 },
	{ "LIST", IRC_MSG_LIST, 0, 0, 1 },
	{ "KICK", IRC_MSG_KICK, 1, 2, 3 },
	{ "VERSION", IRC_MSG_VERSION, 0, 0, 1 },
	{ "STATS", IRC_MSG_STATS, 0, 0, 2 },
	{ "LINKS", IRC_MSG_LINKS, 0, 0, 2 },
	{ "TIME", IRC_MSG_TIME, 0, 0, 1 },
	{ "CONNECT", IRC_MSG_CONNECT, 0, 2, 3 },
	{ "TRACE", IRC_MSG_TRACE, 0, 0, 1 },
	{ "ADMIN", IRC_MSG_ADMIN, 0, 0, 1 },
	{ "INFO", IRC_MSG_INFO, 0, 0, 1 },
	{ "NOTICE", IRC_MSG_NOTICE, 1, 2, 2 },
	{ "PRIVMSG", IRC_MSG_PRIVMSG, 1, 2, 2 },
	{ "INVITE", IRC_MSG_INVITE, 0, 2, 2 },
	{ "WHO", IRC_MSG_WHO, 0, 1, 2 },
	{ "WHOIS", IRC_MSG_WHOIS, 0, 1, 2 },
	{ "WHOWAS", IRC_MSG_WHOWAS, 0, 1, 3 },
	{ "PING", IRC_MSG_PING, 1, 1, 2 },
	{ "PONG", IRC_MSG_PONG, 1, 1, 2 },
	{ "MOTD", IRC_MSG_MOTD, 0, 0, 1 },
	{ "LUSERS", IRC_MSG_LUSERS, 0, 0, 2 },
	{ "ERROR", IRC_MSG_ERROR, 1, 1, 1 },
	{ "SQUIT", IRC_MSG_SQUIT, 1, 2, 2 },
	{ "SERVLIST", IRC_MSG_SERVLIST, 0, 0, 2 },
	{ "SQUERY", IRC_MSG_SQUERY, 1, 2, 2 },
	{ "KILL", IRC_MSG_KILL, 1, 2, 2 },
	{ NULL, 0 }
};

static struct irc_msg *msg_create(int, int, char *, char *, int, char **, int, struct irc_msg_ctcp *);
static int msg_parse_ctcps(char *, char *, int, struct irc_msg_ctcp *, int);

inline static struct irc_msg_commands *msg_get_command(int);
inline static int msg_ctoa(int, char *);
inline static char *msg_uppercase(char *);

/**
 * Allocate and initialize an irc_msg structure using the values
 * given.  All strings are copied to newly allocated memory.
 */
struct irc_msg *irc_create_msg(int cmd, char *nick, char *host, int num_params, int num_ctcps, ...)
{
	int j;
	va_list va;
	char *params[IRC_MAX_PARAMS];
	struct irc_msg_commands *info;
	struct irc_msg_ctcp ctcps[IRC_MAX_CTCPS];

	if (num_params >= IRC_MAX_PARAMS)
		return(NULL);

	va_start(va, num_ctcps);
	for (j = 0; j < num_params; j++)
		params[j] = va_arg(va, char *);
	if ((info = msg_get_command(cmd))) {
		if (num_params < info->min_params) {
			IRC_ERROR_JOINPOINT(IRC_ERR_MSG_NOT_ENOUGH_PARAMS, info->name)
			return(NULL);
		}
		else if (info->max_params && (num_params > info->max_params)) {
			IRC_ERROR_JOINPOINT(IRC_ERR_MSG_TOO_MANY_PARAMS, info->name)
			return(NULL);
		}
	}
	for (j = 0; j < num_ctcps; j++) {
		ctcps[j].tag = va_arg(va, char *);
		ctcps[j].args = va_arg(va, char *);
	}
	return(msg_create((info && (num_params == info->max_params)) ? info->use_text : 0, cmd, nick, host, num_params, params, num_ctcps, ctcps));
}

/**
 * Create an exact copy of the given message and return a pointer or
 * NULL on error.
 */
struct irc_msg *irc_duplicate_msg(struct irc_msg *msg)
{
	struct irc_msg *dup;

	if (!msg || !(dup = msg_create(msg->text ? 1 : 0, msg->cmd, msg->nick, msg->host, msg->num_params, msg->params, msg->num_ctcps, msg->ctcps)))
		return(NULL);
	dup->time = msg->time;
	return(dup);
}

/**
 * Free the resources used by the given msg.
 */
int irc_destroy_msg(struct irc_msg *msg)
{
	memory_free(msg);
	return(0);
}

/**
 * Parse the given string and create and return the corresponding irc_msg.  The
 * string will be clobbered but values will be copied into the returned msg.
 * If an error occurs, NULL is returned.
 */
struct irc_msg *irc_unmarshal_msg(char *str)
{
	int i = 0;
	int cmd;
	char *tmp;
	int bitflags = 0;
	char *nick = NULL;
	char *host = NULL;
	char buffer[IRC_MAX_MSG];
	char *params[IRC_MAX_PARAMS];
	int num_params = 0, num_ctcps = 0;
	struct irc_msg_ctcp ctcps[IRC_MAX_CTCPS];

	/* Parse the nick and server if it exists */
	if (str[i] == ':') {
		nick = &str[++i];
		while ((str[i] != '!') && (str[i] != ' ') && (str[i] != '\0'))
			i++;
		if (str[i] == '\0')
			return(NULL);
		if (str[i] == '!') {
			str[i] = '\0';
			host = &str[++i];
			while ((str[i] != ' ') && (str[i] != '\0'))
				i++;
			if (str[i] == '\0')
				return(NULL);
		}
		str[i] = '\0';
		i++;
	}

	/* Parse the command */
	tmp = &str[i];
	while (str[i] != ' ')
		i++;
	str[i++] = '\0';
	cmd = irc_command_number(tmp);

	/* Parse the parameters */
	params[num_params] = &str[i];
	for (; ((str[i] != '\r') && (str[i] != '\n') && (str[i] != '\0')); i++) {
		if (str[i] == ':') {
			bitflags = 1;
			params[num_params] = &str[++i];
			while ((str[i] != 0x0d) && (str[i] != 0x0a) && (str[i] != '\0'))
				i++;
			str[i] = '\0';
			if ((cmd == IRC_MSG_PRIVMSG) || (cmd == IRC_MSG_NOTICE)) {
				num_ctcps = msg_parse_ctcps(params[num_params], buffer, IRC_MAX_MSG, ctcps, IRC_MAX_CTCPS);
				params[num_params] = buffer;
			}
			break;
		}
		else if (str[i] == ' ') {
			str[i] = '\0';
			if (++num_params >= IRC_MAX_PARAMS)
				return(NULL);
			params[num_params] = &str[i + 1];
		}
	}
	str[i] = '\0';

	return(msg_create(bitflags, cmd, nick, host, ++num_params, params, num_ctcps, ctcps));
}

/**
 * Convert the given message back into a string representation of the message
 * using the given buffer with the size given and return the length of the
 * message or 0 on error.
 */
int irc_marshal_msg(struct irc_msg *msg, char *buffer, int size)
{
	int j;
	int i = 0;
	char *cmd;

	size -= 2;		/* Reserve room for line terminator */

	if (msg->nick || msg->host) {
		buffer[i++] = ':';
		if (msg->nick) {
			strncpy(&buffer[i], msg->nick, size - i);
			i += strlen(msg->nick);
			buffer[i++] = '!';
		}
		strncpy(&buffer[i], msg->host, size - i);
		i += strlen(msg->host);
		buffer[i++] = ' ';
	}

	if (i >= size)
		return(0);

	if ((cmd = irc_command_name(msg->cmd))) {
		strncpy(&buffer[i], cmd, size - i);
		i += strlen(cmd);
	}
	else
		i += msg_ctoa(msg->cmd, &buffer[i]);

	for (j = 0; j < msg->num_params; j++) {
		buffer[i++] = ' ';
		if (i >= size)
			return(0);
		if (msg->params[j] == msg->text) {
			buffer[i++] = ':';
			strncpy(&buffer[i], msg->params[j], size - i);
			i += strlen(msg->params[j]);
			break;
		}
		else
			strncpy(&buffer[i], msg->params[j], size - i);
		i += strlen(msg->params[j]);
	}
	for (j = 0; j < msg->num_ctcps; j++) {
		buffer[i++] = 0x01;
		strncpy(&buffer[i], msg->ctcps[j].tag, size - i);
		i += strlen(msg->ctcps[j].tag);
		if (msg->ctcps[j].args) {
			buffer[i++] = ' ';
			strncpy(&buffer[i], msg->ctcps[j].args, size - i);
			i += strlen(msg->ctcps[j].args);
		}
		buffer[i++] = 0x01;
	}
	buffer[i++] = '\r';
	buffer[i++] = '\n';

	return(i);
}

/**
 * Search the list of commands for the command number given and return the
 * corresponding command name string.
 */
char *irc_command_name(int cmd)
{
	int i = 0;

	while (msg_commands[i].name) {
		if (msg_commands[i].cmd == cmd)
			return(msg_commands[i].name);
		i++;
	}
	return(NULL);
}

/**
 * Determine the command number corresponding to the given command string.
 */
short irc_command_number(char *str)
{
	int i = 0;
	int cmd = 0;

	if ((str[0] >= 0x30) && (str[0] <= 0x39))
		return(atoi(str));

	msg_uppercase(str);
	while (msg_commands[i].name) {
		if (!strcmp(str, msg_commands[i].name))
			return(msg_commands[i].cmd);
		i++;
	}

	return(cmd);
}

/*** Local Functions ***/

/**
 * Create a new message based on the given values.
 */
static struct irc_msg *msg_create(int bitflags, int cmd, char *nick, char *host, int num_params, char **params, int num_ctcps, struct irc_msg_ctcp *ctcps)
{
	char *buffer;
	int i = 0, j, size;
	struct irc_msg *msg;
	int nick_len, host_len, param_len = 0, ctcp_len = 0;

	nick_len = nick ? strlen(nick) + 1 : 0;
	host_len = host ? strlen(host) + 1 : 0;
	for (j = 0; j < num_params; j++)
		param_len += strlen(params[j]) + 1;
	for (j = 0; j < num_ctcps; j++)
		ctcp_len += strlen(ctcps[j].tag) + 1 + (ctcps[j].args ? (strlen(ctcps[j].args) + 1) : 0);
	size = sizeof(struct irc_msg) + (sizeof(char *) * num_params) + (sizeof(struct irc_msg_ctcp) * num_ctcps) + nick_len + host_len + param_len + ctcp_len;

	if (!(msg = (struct irc_msg *) memory_alloc(size)))
		return(NULL);
	memset(msg, '\0', size);
	msg->size = size;
	msg->time = time(NULL);
	msg->cmd = cmd;
	msg->num_params = num_params;
	msg->params = num_params ? (char **) (((size_t) msg) + sizeof(struct irc_msg)) : NULL;
	msg->num_ctcps = num_ctcps;
	msg->ctcps = num_ctcps ? (struct irc_msg_ctcp *) (((size_t) msg) + sizeof(struct irc_msg) + (sizeof(char *) * num_params)) : NULL;

	buffer = (char *) (((size_t) msg) + sizeof(struct irc_msg) + (sizeof(char *) * num_params) + (sizeof(struct irc_msg_ctcp) * num_ctcps));

	if (nick_len) {
		msg->nick = &buffer[i];
		i += nick_len;
		strcpy(msg->nick, nick);
	}
	if (host_len) {
		msg->host = &buffer[i];
		i += host_len;
		strcpy(msg->host, host);
	}
	if (num_params) {
		for (j = 0; j < num_params; j++) {
			msg->params[j] = &buffer[i];
			i += strlen(params[j]) + 1;
			strcpy(msg->params[j], params[j]);
		}
		if (bitflags)
			msg->text = msg->params[num_params - 1];
	}
	if (num_ctcps) {
		for (j = 0; j < num_ctcps; j++) {
			msg->ctcps[j].tag = &buffer[i];
			i += strlen(ctcps[j].tag) + 1;
			strcpy(msg->ctcps[j].tag, ctcps[j].tag);
			if (!ctcps[j].args || (ctcps[j].args[0] == '\0'))
				ctcps[j].args = NULL;
			else {
				msg->ctcps[j].args = &buffer[i];
				i += strlen(ctcps[j].args) + 1;
				strcpy(msg->ctcps[j].args, ctcps[j].args);
			}
		}
	}
	return(msg);
}

/**
 * Parse the ctcp messages out of the given string and store them in the
 * given array of ctcps up to the maximum number of available ctcps.
 */
static int msg_parse_ctcps(char *str, char *buffer, int max_buffer, struct irc_msg_ctcp *ctcps, int max_ctcps)
{
	int i, j = 0, k = 0;

	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == 0x01) {
			if (k >= max_ctcps)
				return(k);
			ctcps[k].tag = &str[++i];
			for (; (str[i] != '\0') && (str[i] != ' ') && (str[i] != 0x01); i++) ;
			if (str[i] == 0x01) {
				str[i] = '\0';
				ctcps[k].args = NULL;
			}
			else {
				str[i] = '\0';
				ctcps[k].args = &str[++i];
				for (; (str[i] != '\0') && (str[i] != 0x01); i++) ;
				str[i++] = '\0';
			}
			k++;
		}
		else {
			buffer[j++] = str[i];
			if (j >= max_buffer)
				return(k);
		}
	}
	buffer[j] = '\0';
	return(k);
}

/**
 * Search the list of commands for the command number given and return 1
 * if the command should have a quoted parameter at the end or 0 otherwise.
 */
inline static struct irc_msg_commands *msg_get_command(int cmd)
{
	int i;

	for (i = 0; msg_commands[i].name; i++) {
		if (msg_commands[i].cmd == cmd)
			return(&msg_commands[i]);
	}
	return(NULL);
}

/**
 * Convert the given number into a string representation stored in the
 * given buffer.  The length is returned or 0 on error (always 3).
 */
inline static int msg_ctoa(int num, char *str)
{
	int i = 0, mul = 100;

	if (!str)
		return(0);

	for (i = 0; i < 3; i++) {
		str[i] = (num / mul) + 0x30;
		num -= (num / mul) * mul;
		mul /= 10;
	}
	str[i] = '\0';

	return(i);
}

/**
 * Convert all lowercase letters in the string into uppercase letters and
 * return a pointer to the string.
 */
inline static char *msg_uppercase(char *str)
{
	int i;

	for (i = 0; str[i] != '\0'; i++) {
		if ((str[i] >= 0x61) && (str[i] <= 0x7a))
			str[i] = str[i] - 0x20;
	}
	return(str);
}


