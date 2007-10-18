/*
 * Header Name:		msg.h
 * Description:		Message Manager Header
 */

#ifndef _STUTTER_MODULES_IRC_MSG_H
#define _STUTTER_MODULES_IRC_MSG_H

#include <time.h>
#include <stutter/queue.h>
#include <stutter/modules/irc/commands.h>

#define IRC_MAX_MSG		512
#define IRC_MAX_CTCPS		10
#define IRC_MAX_PARAMS		30

struct irc_server;

struct irc_msg_ctcp {
	char *tag;
	char *args;
};

struct irc_msg {
	int size;
	time_t time;
	struct irc_server *server;
	queue_node_v(irc_msg) queue;
	int cmd;
	char *nick;
	char *host;
	int num_params;
	char **params;
	char *text;
	int num_ctcps;
	struct irc_msg_ctcp *ctcps;
};

struct irc_msg *irc_create_msg(int, char *, char *, int, int, ...);
struct irc_msg *irc_duplicate_msg(struct irc_msg *);
int irc_destroy_msg(struct irc_msg *);
struct irc_msg *irc_unmarshal_msg(char *);
int irc_marshal_msg(struct irc_msg *, char *, int);

char *irc_command_name(int);
short irc_command_number(char *);

#endif


