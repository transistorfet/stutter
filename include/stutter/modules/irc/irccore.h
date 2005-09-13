/*
 * Header Name:		irccore.h
 * Version:		0.1
 * Description:		IRC Interface Manager Header
 */

#ifndef _IRC_IRCCORE_H
#define _IRC_IRCCORE_H

#include <nit/callback.h>
#include <modules/irc/msg.h>
#include <modules/irc/user.h>
#include <modules/irc/server.h>
#include <modules/irc/channel.h>

struct irc_s {
	struct list_s *servers;
	struct irc_channel *current;
	void *last_widget;
// TODO this is for variable dispatch
//	struct list_s *msgs;
};

#define irc_current_server()			( irc_current_channel() ? irc->current->server : NULL )

// TODO this is for variable dispatch
//#define irc_add_message(msg, func, ptr)		list_add(irc->msgs, create_callback(0, 0, (void *) msg, (callback_t) func, (void *) ptr))

extern struct irc_s *irc;

int init_irc(void);
int release_irc(void);

int irc_connect(char *, int, char *, void *);
int irc_disconnect(struct irc_server *);
int irc_broadcast_msg(struct irc_msg *);
int irc_private_msg(struct irc_server *, char *, char *);
int irc_notice(struct irc_server *, char *, char *);

struct irc_channel *irc_current_channel(void);
struct irc_channel *irc_find_channel(char *);
struct irc_channel *irc_find_channel_window(void *);

#endif


