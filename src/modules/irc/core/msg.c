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
#include <stutter/string.h>
#include <stutter/memory.h>
#include <stutter/object.h>
#include <stutter/output.h>
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

struct object_type_s irc_msg_type = {
	NULL,
	"irc_msg",
	sizeof(struct irc_msg),
	NULL,
	(object_init_t) NULL,
	(object_release_t) irc_msg_release
};

static int msg_parse_ctcps(char *, char *, int, struct irc_msg_ctcp *, int);

inline static struct irc_msg_commands *msg_get_command(int);
inline static int msg_ctoa(int, char *);
inline static char *msg_uppercase(char *);

void irc_msg_release(struct irc_msg *msg)
{
	int i;

	if (msg->nick)
		destroy_string(msg->nick);
	if (msg->host)
		destroy_string(msg->host);
	for (i = 0; i < msg->num_params; i++) {
		if (msg->params[i])
			destroy_string(msg->params[i]);
	}

	for (i = 0; i < msg->num_ctcps; i++) {
		if (msg->ctcps[i].tag)
			destroy_string(msg->ctcps[i].tag);
		if (msg->ctcps[i].args)
			destroy_string(msg->ctcps[i].args);
	}
}

/**
 * Allocate and initialize an irc_msg structure using the values
 * given.  All strings are copied to newly allocated memory.
 */
struct irc_msg *irc_create_msg(int cmd, const char *nick, const char *host, int num_params, int num_ctcps, ...)
{
	int i;
	char *str;
	va_list va;
	struct irc_msg *msg;
	struct irc_msg_commands *info;

	if (num_params >= IRC_MAX_PARAMS)
		return(NULL);
	if ((info = msg_get_command(cmd))) {
		if (num_params < info->min_params) {
			OUTPUT_ERROR(IRC_ERR_MSG_NOT_ENOUGH_PARAMS, info->name);
			return(NULL);
		}
		else if (info->max_params && (num_params > info->max_params)) {
			OUTPUT_ERROR(IRC_ERR_MSG_TOO_MANY_PARAMS, info->name);
			return(NULL);
		}
	}

	if (!(msg = IRC_MSG(create_object(&irc_msg_type, ""))))
		return(NULL);
	msg->time = time(NULL);
	msg->cmd = cmd;
	msg->num_params = num_params;
	msg->num_ctcps = num_ctcps;

	va_start(va, num_ctcps);
	for (i = 0; i < num_params; i++) {
		str = va_arg(va, char *);
		msg->params[i] = duplicate_string(str ? str : "");
	}
	for (i = 0; i < num_ctcps; i++) {
		str = va_arg(va, char *);
		msg->ctcps[i].tag = duplicate_string(str ? str : "");
		str = va_arg(va, char *);
		msg->ctcps[i].args = duplicate_string(str ? str : "");
	}
	if (info->use_text && msg->num_params)
		msg->text = msg->params[msg->num_params - 1];
	va_end(va);
	return(msg);
}

/**
 * Create an exact copy of the given message and return a pointer or
 * NULL on error.
 */
struct irc_msg *irc_duplicate_msg(struct irc_msg *msg)
{
	int i;
	struct irc_msg *dup;

	if (!(dup = IRC_MSG(create_object(&irc_msg_type, ""))))
		return(NULL);
	dup->time = msg->time;
	dup->server = msg->server;
	dup->cmd = msg->cmd;
	dup->nick = duplicate_string(msg->nick);
	dup->host = duplicate_string(msg->host);

	dup->num_params = msg->num_params;
	for (i = 0; i < msg->num_params; i++)
		dup->params[i] = duplicate_string(msg->params[i]);
	dup->num_ctcps = msg->num_ctcps;
	for (i = 0; i < msg->num_ctcps; i++) {
		dup->ctcps[i].tag = duplicate_string(msg->ctcps[i].tag);
		dup->ctcps[i].args = duplicate_string(msg->ctcps[i].args);
	}
	if (msg->text && dup->num_params)
		dup->text = dup->params[dup->num_params - 1];
	return(dup);
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
	struct irc_msg *msg;

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

	/** Create the actual message object and start filling it in */
	if (!(msg = IRC_MSG(create_object(&irc_msg_type, ""))))
		return(NULL);
	msg->time = time(NULL);
	msg->cmd = cmd;
	msg->nick = duplicate_string(nick);
	msg->host = duplicate_string(host);
	msg->num_params = ++num_params;
	msg->num_ctcps = num_ctcps;

	for (i = 0; i < num_params; i++)
		msg->params[i] = duplicate_string(params[i]);
	for (i = 0; i < num_ctcps; i++) {
		msg->ctcps[i].tag = duplicate_string(ctcps[i].tag);
		msg->ctcps[i].args = duplicate_string(ctcps[i].tag);
	}
	if (bitflags && msg->num_params)
		msg->text = msg->params[msg->num_params - 1];
	return(msg);
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


