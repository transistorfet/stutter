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
int irc_cmd_ctcp(char *, char *);
int irc_cmd_disconnect(char *, char *);
int irc_cmd_join(char *, char *);
int irc_cmd_leave(char *, char *);
int irc_cmd_me(char *, char *);
int irc_cmd_msg(char *, char *);
int irc_cmd_names(char *, char *);
int irc_cmd_nick(char *, char *);
int irc_cmd_notice(char *, char *);
int irc_cmd_ping(char *, char *);
int irc_cmd_reconnect(char *, char *);
int irc_cmd_say(char *, char *);
int irc_cmd_server(char *, char *);
int irc_cmd_topic(char *, char *);
int irc_cmd_whois(char *, char *);

/* Messages */
int irc_msg_current(char *, struct irc_msg *);
int irc_msg_default(char *, struct irc_msg *);
int irc_msg_join(char *, struct irc_msg *);
int irc_msg_kick(char *, struct irc_msg *);
int irc_msg_names(char *, struct irc_msg *);
int irc_msg_nick(char *, struct irc_msg *);
int irc_msg_notice(char *, struct irc_msg *);
int irc_msg_part(char *, struct irc_msg *);
int irc_msg_privmsg(char *, struct irc_msg *);
int irc_msg_quit(char *, struct irc_msg *);
int irc_msg_topic(char *, struct irc_msg *);

int irc_msg_ctcp_action(char *, struct irc_msg *);
int irc_msg_ctcp_ping(char *, struct irc_msg *);
int irc_msg_ctcp_version(char *, struct irc_msg *);

/* Utils */
int irc_dispatch_msg(char *, struct irc_msg *);
int irc_format_msg(struct irc_msg *, char *, char *, int);
int irc_sig_quit(void *, char *);

struct irc_server *irc_current_server(void);
struct irc_channel *irc_current_channel(void);
int irc_stringify_nick(void *, char *, int);
int irc_stringify_channel(void *, char *, int);

#endif


