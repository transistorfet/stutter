/*
 * Header Name:		server.h
 * Version:		0.1
 * Description:		Server Interface Manager Header
 */

#ifndef _SERVER_H
#define _SERVER_H

#include CONFIG_H
#include <nit/net.h>
#include <nit/types.h>
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
	struct irc_channel_list *channels;
};

int init_irc_server(void);
int release_irc_server(void);
struct irc_server *irc_server_connect(char *, int, char *, void *);
int irc_server_reconnect(struct irc_server *);
int irc_server_disconnect(struct irc_server *);
struct irc_channel *irc_server_find_window(void *);

int irc_send_msg(struct irc_server *, struct irc_msg *);
struct irc_msg *irc_receive_msg(struct irc_server *);
int irc_broadcast_msg(struct irc_msg *);

struct irc_channel *irc_join_channel(struct irc_server *, char *, void *);
int irc_leave_channel(struct irc_server *, char *);
int irc_change_nick(struct irc_server *, char *);
int irc_private_msg(struct irc_server *, char *, char *);
int irc_notice(struct irc_server *, char *, char *);

#endif


