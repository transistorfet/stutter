/*
 * Header Name:		server.h
 * Version:		0.1
 * Description:		Server Interface Manager Header
 */

#ifndef _SERVER_H
#define _SERVER_H

#include CONFIG_H
#include <nit/net.h>
#include <nit/list.h>
#include <modules/irc/msg.h>
#include <modules/irc/channel.h>

#define IRC_SERVER_STATUS_CHANNEL	"status"

#define IRC_SBF_CONNECTED		0x01

struct irc_server {
	int bitflags;
	char *address;
	int port;

	network_t net;
	time_t last_ping;
	char nick[IRC_MAX_NICK];
	struct irc_channel *status;
	struct list_s *channels;
};

#define irc_create_server_list()		create_list(0, NULL, (destroy_t) irc_server_disconnect)
#define irc_get_channel(server, name)		((struct irc_channel *) list_find(server->channels, name, 0))

struct irc_server *irc_server_connect(char *, int, char *);
int irc_server_reconnect(struct irc_server *);
int irc_server_disconnect(struct irc_server *);
int irc_send_msg(struct irc_server *, struct irc_msg *);
struct irc_msg *irc_receive_msg(struct irc_server *);
int irc_dispatch_msg(struct irc_server *, struct irc_msg *);

struct irc_channel *irc_join_channel(struct irc_server *, char *, void *);
int irc_leave_channel(struct irc_server *, char *);
int irc_change_nick(struct irc_server *, char *);

#endif


