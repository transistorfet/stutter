/*
 * Header Name:		msg.h
 * Version:		0.1
 * Description:		Message Manager Header
 */

#ifndef _STUTTER_MODULES_IRC_MSG_H
#define _STUTTER_MODULES_IRC_MSG_H

#include <time.h>
#include <stutter/modules/irc/commands.h>

#define IRC_MAX_MSG		512
#define IRC_MAX_PARAMS		30

struct irc_server;

struct irc_msg {
	int size;
	time_t time;
	struct irc_server *server;
	short cmd;
	char *nick;
	char *host;
	short num_params;
	char **params;
	char *text;
};

struct irc_msg *irc_create_msg(short, char *, char *, short, ...);
struct irc_msg *irc_duplicate_msg(struct irc_msg *);
int irc_destroy_msg(struct irc_msg *);
struct irc_msg *irc_parse_msg(char *);
int irc_collapse_msg(struct irc_msg *, char *, int);

char *irc_command_name(short);
short irc_command_number(char *);

#endif


