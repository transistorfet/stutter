/*
 * Header Name:		irc.h
 * Version:		0.1
 * Description:		IRC Module Header
 */

#ifndef _MODULES_IRC_H
#define _MODULES_IRC_H

#include <nit/string.h>
#include <modules/irc/msg.h>
#include <modules/irc/user.h>
#include <modules/irc/server.h>
#include <modules/irc/channel.h>

int init_irc(void);
int release_irc(void);

/* Commands */
int irc_cmd_join(char *, char *);
int irc_cmd_leave(char *, char *);
int irc_cmd_quit(char *, char *);
int irc_cmd_msg(char *, char *);
int irc_cmd_me(char *, char *);
int irc_cmd_nick(char *, char *);
int irc_cmd_names(char *, char *);
int irc_cmd_notice(char *, char *);
int irc_cmd_say(char *, char *);
int irc_cmd_server(char *, char *);
int irc_cmd_disconnect(char *, char *);

/* Messages */
int irc_msg_topic(struct irc_server *, struct irc_msg *);
int irc_msg_nick(struct irc_server *, struct irc_msg *);
int irc_msg_join(struct irc_server *, struct irc_msg *);
int irc_msg_kick(struct irc_server *, struct irc_msg *);
int irc_msg_names(struct irc_server *, struct irc_msg *);
int irc_msg_notice(struct irc_server *, struct irc_msg *);
int irc_msg_part(struct irc_server *, struct irc_msg *);
int irc_msg_privmsg(struct irc_server *, struct irc_msg *);
int irc_msg_quit(struct irc_server *, struct irc_msg *);
int irc_msg_default(struct irc_server *, struct irc_msg *);

/* Utils */
int irc_dispatch_msg(struct irc_server *, struct irc_msg *);
string_t irc_format_msg(struct irc_msg *, char *);

struct irc_server *irc_current_server(void);
struct irc_channel *irc_current_channel(void);
string_t irc_stringify_nick(void *);
string_t irc_stringify_channel(void *);

#endif


