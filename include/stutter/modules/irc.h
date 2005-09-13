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
#include <modules/irc/irccore.h>

/* Commands */
int irc_cmd_join(char *, char *);
int irc_cmd_leave(char *, char *);
int irc_cmd_quit(char *, char *);
int irc_cmd_msg(char *, char *);
int irc_cmd_me(char *, char *);
int irc_cmd_nick(char *, char *);
int irc_cmd_notice(char *, char *);
int irc_cmd_say(char *, char *);

/* Messages */
int irc_msg_topic(char *, struct irc_msg *);
int irc_msg_nick(char *, struct irc_msg *);
int irc_msg_join(char *, struct irc_msg *);
int irc_msg_kick(char *, struct irc_msg *);
int irc_msg_names(char *, struct irc_msg *);
int irc_msg_notice(char *, struct irc_msg *);
int irc_msg_part(char *, struct irc_msg *);
int irc_msg_privmsg(char *, struct irc_msg *);
int irc_msg_quit(char *, struct irc_msg *);
int irc_msg_default(char *, struct irc_msg *);

/* Utils */
int irc_status_bar(char *, void *);
int irc_msg_dispatch(char *, struct irc_msg *);
string_t irc_format_msg(struct irc_msg *, char *, ...);

#endif


