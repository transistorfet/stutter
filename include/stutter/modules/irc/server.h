/*
 * Header Name:		server.h
 * Description:		Server Interface Manager Header
 */

#ifndef _STUTTER_MODULES_IRC_SERVER_H
#define _STUTTER_MODULES_IRC_SERVER_H

#include <stutter/queue.h>
#include <stutter/string.h>
#include <stutter/object.h>
#include <stutter/variable.h>
#include <stutter/frontend/net.h>
#include <stutter/modules/irc/msg.h>
#include <stutter/modules/irc/channel.h>

#define IRC_SERVER_STATUS_CHANNEL	"status"

#define IRC_SBF_CONNECTED		0x01

#define IRC_SERVER(ptr)			( (struct irc_server *) (ptr) )

struct irc_server {
	struct variable_s variable;
	struct irc_server *next;
	int bitflags;
	string_t address;
	int port;

	fe_network_t net;
	time_t last_ping;
	int attempts;
	string_t nick;
	struct queue_s *send_queue;
	struct irc_channel *status;
	struct irc_channel_list channels;
};

extern struct variable_type_s irc_server_type;

int init_irc_server(void);
int release_irc_server(void);

int irc_server_init(struct irc_server *, const char *, va_list);
void irc_server_release(struct irc_server *);

int irc_server_connect(struct irc_server *, const char *, int);
int irc_server_reconnect(struct irc_server *);
int irc_server_disconnect(struct irc_server *);
struct irc_server *irc_find_server(const char *);

int irc_send_msg(struct irc_server *, struct irc_msg *);
struct irc_msg *irc_receive_msg(struct irc_server *);
int irc_broadcast_msg(struct irc_msg *);

int irc_join_channel(struct irc_server *, const char *);
int irc_leave_channel(struct irc_server *, const char *);
int irc_change_nick(struct irc_server *, const char *);
int irc_private_msg(struct irc_server *, const char *, const char *);
int irc_notice(struct irc_server *, const char *, const char *);
int irc_ctcp_msg(struct irc_server *, const char *, const char *, const char *);
int irc_ctcp_reply(struct irc_server *, const char *, const char *, const char *);

#endif


