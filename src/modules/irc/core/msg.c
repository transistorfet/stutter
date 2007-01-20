/*
 * Module Name:		msg.c
 * Version:		0.1
 * Module Requirements:	utils
 * Description:		Message Manager
 */


#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/modules/irc/msg.h>

#define msg_copy_addr_m(dest, src, size)	\
	if (size) {				\
		dest = &buffer[i];		\
		i += size;			\
		strncpy(dest, src, size);	\
	}

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

static struct irc_msg *msg_create(short, short, char *, char *, short, char **);
static struct irc_msg_commands *msg_get_command(short);
static int msg_ctoa(int, char *);
static char *msg_uppercase(char *);

/**
 * Allocate and initialize an irc_msg structure using the values
 * given.  All strings are copied to newly allocated memory.
 */
struct irc_msg *irc_create_msg(short cmd, char *nick, char *host, short num_params, ...)
{
	int j;
	va_list va;
	char *params[IRC_MAX_PARAMS];
	struct irc_msg_commands *info;

	if (num_params >= IRC_MAX_PARAMS)
		return(NULL);

	va_start(va, num_params);
	for (j = 0;j < num_params;j++)
		params[j] = va_arg(va, char *);
	if (info = msg_get_command(cmd)) {
		if (num_params < info->min_params) {
			IRC_ERROR_JOINPOINT(IRC_ERR_MSG_NOT_ENOUGH_PARAMS, info->name)
			return(NULL);
		}
		else if (info->max_params && (num_params > info->max_params)) {
			IRC_ERROR_JOINPOINT(IRC_ERR_MSG_TOO_MANY_PARAMS, info->name)
			return(NULL);
		}
	}
	return(msg_create((info && (num_params == info->max_params)) ? info->use_text : 0, cmd, nick, host, num_params, params));
}

/**
 * Create an exact copy of the given message and return a pointer or
 * NULL on error.
 */
struct irc_msg *irc_duplicate_msg(struct irc_msg *msg)
{
	struct irc_msg *dup;

	if (!msg || !(dup = msg_create(msg->text ? 1 : 0, msg->cmd, msg->nick, msg->host, msg->num_params, msg->params)))
		return(NULL);
	dup->time = msg->time;
	return(dup);
}

/**
 * Free the resources used by the given msg.
 */
int irc_destroy_msg(struct irc_msg *msg)
{
	free(msg);
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
	short cmd;
	char *tmp;
	int bitflags = 0;
	char *nick = NULL;
	char *host = NULL;
	short num_params = 0;
	char *params[IRC_MAX_PARAMS] = { NULL };

	/* Parse the nick and server if it exists */
	if (str[i] == ':') {
		host = &str[++i];
		while ((str[i] != '!') && (str[i] != ' ') && (str[i] != '\0'))
			i++;
		if (str[i] == '\0')
			return(NULL);
		if (str[i] == '!') {
			str[i] = '\0';
			nick = host;
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
	for (;((str[i] != '\r') && (str[i] != '\n') && (str[i] != '\0'));i++) {
		if (str[i] == ':') {
			bitflags = 1;
			params[num_params] = &str[++i];
			while ((str[i] != 0x0d) && (str[i] != 0x0a) && (str[i] != '\0'))
				i++;
			str[i] = '\0';
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

	return(msg_create(bitflags, cmd, nick, host, ++num_params, params));
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

	if (cmd = irc_command_name(msg->cmd)) {
		strncpy(&buffer[i], cmd, size - i);
		i += strlen(cmd);
	}
	else
		i += msg_ctoa(msg->cmd, &buffer[i]);

	for (j = 0;j < msg->num_params;j++) {
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
	buffer[i++] = '\r';
	buffer[i++] = '\n';

	return(i);
}

/**
 * Search the list of commands for the command number given and return the
 * corresponding command name string.
 */
char *irc_command_name(short cmd)
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
static struct irc_msg *msg_create(short bitflags, short cmd, char *nick, char *host, short num_params, char **params)
{
	char *buffer;
	struct irc_msg *msg;
	int i = 0, param_len = 0;
	int j, size, nick_len, host_len;

	nick_len = nick ? strlen(nick) + 1 : 0;
	host_len = host ? strlen(host) + 1 : 0;
	for (j = 0;j < num_params;j++)
		param_len += strlen(params[j]) + 1;
	size = sizeof(struct irc_msg) + (sizeof(char *) * num_params) + nick_len + host_len + param_len;

	if (!(msg = (struct irc_msg *) malloc(size)))
		return(NULL);
	memset(msg, '\0', size);
	msg->size = size;
	msg->time = time(NULL);
	msg->cmd = cmd;
	msg->num_params = num_params;

	buffer = (char *) (((size_t) msg) + sizeof(struct irc_msg));

	if (nick_len) {
		msg->nick = &buffer[i];
		i += nick_len;
		strncpy(msg->nick, nick, nick_len);
	}
	if (host_len) {
		msg->host = &buffer[i];
		i += host_len;
		strncpy(msg->host, host, host_len);
	}

	if (num_params) {
		msg->params = (char **) &buffer[i];
		i += sizeof(char *) * num_params;
		for (j = 0;j < num_params;j++) {
			param_len = strlen(params[j]) + 1;
			msg_copy_addr_m(msg->params[j], params[j], param_len);
		}
		if (bitflags)
			msg->text = msg->params[num_params - 1];
	}
	return(msg);
}

/**
 * Search the list of commands for the command number given and return 1
 * if the command should have a quoted parameter at the end or 0 otherwise.
 */
static struct irc_msg_commands *msg_get_command(short cmd)
{
	int i = 0;

	while (msg_commands[i].name) {
		if (msg_commands[i].cmd == cmd)
			return(&msg_commands[i]);
		i++;
	}
	return(NULL);
}

/**
 * Convert the given number into a string representation stored in the
 * given buffer.  The length is returned or 0 on error (always 3).
 */
static int msg_ctoa(int num, char *str)
{
	int i = 0, mul = 100;

	if (!str)
		return(0);

	for (i = 0;i < 3;i++) {
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
static char *msg_uppercase(char *str)
{
	int i;

	for (i = 0;str[i] != '\0';i++)
		if ((str[i] >= 0x61) && (str[i] <= 0x7a))
			str[i] = str[i] - 0x20;
	return(str);
}


