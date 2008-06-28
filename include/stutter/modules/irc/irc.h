/*
 * Header Name:		irc.h
 * Description:		IRC Module Header
 */

#ifndef _STUTTER_MODULES_IRC_H
#define _STUTTER_MODULES_IRC_H

#include <stutter/string.h>
#include <stutter/modules/irc/msg.h>
#include <stutter/modules/irc/user.h>
#include <stutter/modules/irc/server.h>
#include <stutter/modules/irc/channel.h>

int init_irc(void);
int release_irc(void);

/* Commands */
int irc_cmd_complete(char *, char *);
int irc_cmd_ctcp(char *, char *);
int irc_cmd_disconnect(char *, char *);
int irc_cmd_endquery(char *, char *);
int irc_cmd_join(char *, char *);
int irc_cmd_kick(char *, char *);
int irc_cmd_leave(char *, char *);
int irc_cmd_me(char *, char *);
int irc_cmd_mode(char *, char *);
int irc_cmd_msg(char *, char *);
int irc_cmd_names(char *, char *);
int irc_cmd_nick(char *, char *);
int irc_cmd_notice(char *, char *);
int irc_cmd_ping(char *, char *);
int irc_cmd_query(char *, char *);
int irc_cmd_reconnect(char *, char *);
int irc_cmd_say(char *, char *);
int irc_cmd_server(char *, char *);
int irc_cmd_topic(char *, char *);
int irc_cmd_whois(char *, char *);
int irc_cmd_whowas(char *, char *);

/* Messages */
int irc_msg_current(char *, struct irc_msg *);
int irc_msg_default(char *, struct irc_msg *);
int irc_msg_inuse(char *, struct irc_msg *);
int irc_msg_join(char *, struct irc_msg *);
int irc_msg_kick(char *, struct irc_msg *);
int irc_msg_mode(char *, struct irc_msg *);
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
//int irc_msg_ctcp_unknown(char *, void *, struct irc_msg *);

/* Signal Handlers */
int irc_sig_exec(void *, void *, char *);
int irc_sig_quit(void *, void *, char *);

/* Types */
struct type_s *irc_load_channel_list(void);
struct type_s *irc_load_channel_type(void);
struct type_s *irc_load_server_list(void);
struct type_s *irc_load_server_type(void);
struct type_s *irc_load_user_list(void);
struct type_s *irc_load_user_type(void);

/* Utils */
int irc_dispatch_msg(struct irc_msg *);
int irc_dispatch_ctcp_msg(struct irc_msg *);
int irc_format_msg(struct irc_msg *, char *, char *, int);

struct irc_server *irc_current_server(void);
struct irc_channel *irc_current_channel(void);
int irc_stringify_nick(void *, char *, int);
int irc_stringify_channel(void *, char *, int);

#endif


