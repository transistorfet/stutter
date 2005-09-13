/*
 * Header Name:		msg.h
 * Version:		0.1
 * Description:		Message Manager Header
 */

#ifndef _MSG_H
#define _MSG_H

#include <time.h>
#include <modules/irc/commands.h>

#define IRC_MAX_MSG		512
#define IRC_MAX_PARAMS		30

struct irc_msg {
	int size;
	time_t time;
	short cmd;
	char *nick;
	char *server;
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


