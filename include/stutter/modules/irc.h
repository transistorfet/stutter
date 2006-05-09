/*
 * Header Name:		irc.h
 * Version:		0.1
 * Description:		IRC Module Header
 */

#ifndef _STUTTER_MODULES_IRC_H
#define _STUTTER_MODULES_IRC_H

#include <stutter/lib/string.h>
#include <stutter/modules/irc/msg.h>
#include <stutter/modules/irc/user.h>
#include <stutter/modules/irc/server.h>
#include <stutter/modules/irc/channel.h>

int init_irc(void);
int release_irc(void);

/* Commands */
int irc_cmd_disconnect(char *, char *);
int irc_cmd_join(char *, char *);
int irc_cmd_leave(char *, char *);
int irc_cmd_me(char *, char *);
int irc_cmd_msg(char *, char *);
int irc_cmd_names(char *, char *);
int irc_cmd_nick(char *, char *);
int irc_cmd_notice(char *, char *);
int irc_cmd_ping(char *, char *);
int irc_cmd_quit(char *, char *);
int irc_cmd_say(char *, char *);
int irc_cmd_server(char *, char *);
int irc_cmd_whois(char *, char *);

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
int irc_format_msg(struct irc_msg *, char *, char *, int);

struct irc_server *irc_current_server(void);
struct irc_channel *irc_current_channel(void);
char *irc_stringify_nick(void *);
char *irc_stringify_channel(void *);

#endif


