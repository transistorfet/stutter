/*
 * Header Name:		msg.h
 * Description:		Message Manager Header
 */

#ifndef _STUTTER_MODULES_IRC_MSG_H
#define _STUTTER_MODULES_IRC_MSG_H

#include <time.h>
#include <stutter/string.h>
#include <stutter/object.h>
#include <stutter/modules/irc/commands.h>

#define IRC_MAX_MSG		512
#define IRC_MAX_CTCPS		10
#define IRC_MAX_PARAMS		30

struct irc_server;

struct irc_msg_ctcp {
	string_t tag;
	string_t args;
};

struct irc_msg {
	time_t time;
	struct irc_server *server;
	int cmd;
	string_t nick;
	string_t host;
	string_t text;

	int num_params;
	string_t params[IRC_MAX_PARAMS];

	int num_ctcps;
	struct irc_msg_ctcp ctcps[IRC_MAX_CTCPS];
};

void irc_msg_release(struct irc_msg *);

struct irc_msg *irc_create_msg(int, const char *, const char *, int, int, ...);
struct irc_msg *irc_duplicate_msg(struct irc_msg *);
struct irc_msg *irc_unmarshal_msg(char *);
int irc_marshal_msg(struct irc_msg *, char *, int);

char *irc_command_name(int);
short irc_command_number(char *);

#endif


