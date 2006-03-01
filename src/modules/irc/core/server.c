/*
 * Module Name:		server.c
 * Version:		0.1
 * Module Requirements:	frontend ; memory ; linear ; msg ; channel
 * Description:		Server Interface Manager
 */


#include <string.h>
#include <stdlib.h>

#include CONFIG_H
#include <signal.h>
#include <lib/memory.h>
#include <lib/linear.h>
#include <lib/globals.h>
#include <frontend/net.h>
#include <modules/irc/msg.h>
#include <modules/irc/server.h>
#include <modules/irc/channel.h>

struct irc_server_node {
	struct irc_server server;
	linear_node_v(irc_server_node) sl;
};

static int server_initialized = 0;
static linear_list_v(irc_server_node) server_list;

static int irc_server_receive(struct irc_server *, network_t);
static int server_init_connection(struct irc_server *);

int init_irc_server(void)
{
	if (server_initialized)
		return(1);
	linear_init_v(server_list);
	add_signal("irc_msg_dispatch");
	server_initialized = 1;
	return(0);
}

int release_irc_server(void)
{
	struct irc_server_node *tmp, *cur;

	remove_signal("irc_msg_dispatch");
	linear_destroy_list_v(server_list, sl,
		if (cur->server.channels)
			irc_destroy_channel_list(cur->server.channels);
		memory_free(cur);
	);
	server_initialized = 0;
	return(0);
}

/**
 * Create a new server structure and connect/initialize it to the
 * address:port given.
 */
struct irc_server *irc_server_connect(char *address, int port, char *nick, void *window)
{
	struct irc_server_node *node;

	if (!(node = (struct irc_server_node *) memory_alloc(sizeof(struct irc_server_node) + strlen(address) + 1)))
		return(NULL);
	memset(node, '\0', sizeof(struct irc_server_node));

	node->server.address = (char *) (((size_t) node) + sizeof(struct irc_server_node));
	strcpy(node->server.address, address);
	node->server.port = port;
	strncpy(node->server.nick, nick, IRC_MAX_NICK);

	node->server.channels = irc_create_channel_list();
	node->server.status = irc_add_channel(node->server.channels, IRC_SERVER_STATUS_CHANNEL, window, &node->server);
	linear_add_node_v(server_list, sl, node);

	if (server_init_connection(&node->server) < 0) {
		irc_server_disconnect(&node->server);
		return(NULL);
	}
	return(&node->server);
}

/**
 * Reestablish a connection to the given server
 */
int irc_server_reconnect(struct irc_server *server)
{
	fe_net_disconnect(server->net);
	if (server_init_connection(server)) {
		irc_server_disconnect(server);
		return(-1);
	}
	// TODO reconnect to channels in list
	return(0);
}

/**
 * Disconnect and destroy the server structure given.
 */
int irc_server_disconnect(struct irc_server *server)
{
	linear_remove_node_v(server_list, sl, (struct irc_server_node *) server);
	irc_destroy_channel_list(server->channels);
	fe_net_disconnect(server->net);
	memory_free(server);
	return(0);
}

/**
 * Return the server struct for the given address or return NULL
 * if not connected to that address.
 */
struct irc_server *irc_find_server(char *address)
{
	linear_traverse_list_v(server_list, sl,
		if (!strcmp(cur->server.address, address))
			return(&cur->server);
	);
	return(NULL);
}

/**
 * Return the channel that has the given window handle out of all the channels
 * on all the servers.  If no channel is associated with the given window
 * then NULL is returned.
 */
struct irc_channel *irc_server_find_window(void * window)
{
	struct irc_channel *channel;

	linear_traverse_list_v(server_list, sl,
		if (channel = irc_channel_find_window(cur->server.channels, window))
			return(channel);
	);
	return(NULL);
}

/**
 * Send the given irc message to the given server and return 0 (or -1 on
 * error).
 */
int irc_send_msg(struct irc_server *server, struct irc_msg *msg)
{
	int size;
	char buffer[IRC_MAX_MSG];

	if (!server || !msg || !(size = irc_collapse_msg(msg, buffer, IRC_MAX_MSG)))
		return(-1);
	if (fe_net_send(server->net, buffer, size) != size)
		return(-1);
	return(0);
}

/**
 * Attempt to receive an irc message from the given server and return 0
 * (or -1 on error).
 */
struct irc_msg *irc_receive_msg(struct irc_server *server)
{
	int size;
	struct irc_msg *msg;
	char buffer[IRC_MAX_MSG + 1];

	if (!server || !server->net)
		return(NULL);

	while (1) {
		if ((size = fe_net_receive(server->net, buffer, IRC_MAX_MSG + 1)) < 0) {
			fe_net_disconnect(server->net);
			server->net = NULL;
			return(NULL);
		}
		else if (size == 0)
			return(NULL);
		else if (!strncmp(buffer, "PING", 4)) {
			buffer[1] = 'O';
			if (fe_net_send(server->net, buffer, size) != size)
				return(NULL);
			server->last_ping = time(NULL);
		}
		else
			break;
	}

	if (!(msg = irc_parse_msg(buffer)))
		return(NULL);
	msg->server = server;
	return(msg);
}

/**
 * Send the given message to every server in the server list and return 0
 * on success or -1 if any particular send fails.  If a send fails, the
 * function will continue attempting to send messages.
 */
int irc_broadcast_msg(struct irc_msg *msg)
{
	int ret = 0;

	linear_traverse_list_v(server_list, sl,
		ret = irc_send_msg(&cur->server, msg);
	);
	return(ret);
}


/**
 * Send the join command to the given server in order to join the channel
 * on the server with the given name, create a channel structure for the
 * channel and add it to the server channel list.  NULL is returned if
 * the send fails or a pointer to the channel struct on success.  If the
 * channel already exits, the join command will still be sent but a new
 * channel structure will not be created.
 */
int irc_join_channel(struct irc_server *server, char *name)
{
	struct irc_msg *msg;
	struct irc_channel *channel;

	if (!(msg = irc_create_msg(IRC_MSG_JOIN, NULL, NULL, 1, name)))
		return(-1);
	irc_send_msg(server, msg);
	irc_destroy_msg(msg);
	return(0);
}

/**
 * Leave the channel with the given name on the given server, remove the
 * appropriate channel structure from the server channel list and destroy
 * that structure.  A -1 is returned if the message cannot be created or a
 * 0 otherwise.
 */
int irc_leave_channel(struct irc_server *server, char *name)
{
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_PART, NULL, NULL, 1, name)))
		return(-1);
	irc_send_msg(server, msg);
	irc_destroy_msg(msg);
	return(0);
}

/**
 * Change the nick in the given server to the given nick and return 0
 * on success or -1 on failure.
 */
int irc_change_nick(struct irc_server *server, char *nick)
{
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_NICK, NULL, NULL, 1, nick)))
		return(-1);
	irc_send_msg(server, msg);
	irc_destroy_msg(msg);
	return(0);
}

/**
 * Send the given string to the given channel or nick on the given
 * server and return -1 if the send fail and 0 on success.
 */
int irc_private_msg(struct irc_server *server, char *name, char *text)
{
	int ret;
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_PRIVMSG, NULL, NULL, 2, name, text)))
		return(-1);
	msg->server = server;
	if (!(ret = irc_send_msg(server, msg)))
		signal_emit("irc_msg_dispatch", msg);
	irc_destroy_msg(msg);
	return(ret);
}

/**
 * Send the given string to the given channel or nick on the given
 * server as a notice message and return -1 if the send fail and 0 on
 * success.
 */
int irc_notice(struct irc_server *server, char *name, char *text)
{
	int ret;
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_NOTICE, NULL, NULL, 2, name, text)))
		return(-1);
	msg->server = server;
	if (!(ret = irc_send_msg(server, msg)))
		signal_emit("irc_msg_dispatch", msg);
	irc_destroy_msg(msg);
	return(ret);
}

/*** Local Functions ***/

/**
 * Called by network when data is available from the given socket for
 * the given server.
 */
static int irc_server_receive(struct irc_server *server, network_t net)
{
	struct irc_msg *msg;

	if (msg = irc_receive_msg(server))
		signal_emit("irc_msg_dispatch", msg);
	irc_destroy_msg(msg);
	return(0);
}

/**
 * Initialize communications with the server and return 0 on success
 * or -1 on error.
 */
static int server_init_connection(struct irc_server *server)
{
	int ret = 0;
	struct irc_msg *msg;

	if (server->net)
		return(1);
	if (!(server->net = fe_net_connect(server->address, server->port, (callback_t) irc_server_receive, server)))
		return(-1);

	if (!(msg = irc_create_msg(IRC_MSG_NICK, NULL, NULL, 1, server->nick)))
		return(-1);
	ret = irc_send_msg(server, msg);
	irc_destroy_msg(msg);

	if (!ret) {
		if (!(msg = irc_create_msg(IRC_MSG_USER, NULL, NULL, 4, server->nick, "0", "0", "Person Pants")))
			return(-1);
		ret = irc_send_msg(server, msg);
		irc_destroy_msg(msg);
		if (!ret)
			return(0);
	}
	fe_net_disconnect(server->net);
	server->net = NULL;
	return(-1);
}



