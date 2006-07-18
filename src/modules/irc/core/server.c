/*
 * Module Name:		server.c
 * Version:		0.1
 * Module Requirements:	frontend ; queue ; memory ; linear ; msg ; channel
 * Description:		Server Interface Manager
 */


#include <string.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/lib/queue.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/linear.h>
#include <stutter/lib/globals.h>
#include <stutter/frontend/net.h>
#include <stutter/modules/irc/msg.h>
#include <stutter/modules/irc/server.h>
#include <stutter/modules/irc/channel.h>

struct irc_server_node {
	struct irc_server server;
	linear_node_v(irc_server_node) sl;
};

static int server_initialized = 0;
static linear_list_v(irc_server_node) server_list;

static int irc_server_init_connection(struct irc_server *);
static int irc_server_receive(struct irc_server *, network_t);
static int irc_server_rejoin_channel(struct irc_channel *, struct irc_server *);
static int irc_server_flush_send_queue(struct irc_server *);

int init_irc_server(void)
{
	if (server_initialized)
		return(1);
	linear_init_v(server_list);
	add_signal("irc_dispatch_msg", 0);
	add_signal("irc_dispatch_ctcp", 0);
	server_initialized = 1;
	return(0);
}

int release_irc_server(void)
{
	struct irc_server_node *tmp, *cur;

	remove_signal("irc_dispatch_msg");
	remove_signal("irc_dispatch_ctcp");
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
	queue_init_v(node->server.send_queue);

	node->server.channels = irc_create_channel_list();
	node->server.status = irc_add_channel(node->server.channels, IRC_SERVER_STATUS_CHANNEL, window, &node->server);
	linear_add_node_v(server_list, sl, node);

	if (irc_server_init_connection(&node->server) < 0) {
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
	server->bitflags &= ~IRC_SBF_CONNECTED;
	if (irc_server_init_connection(server)) {
		util_emit_str("irc.error", NULL, ERR_MSG_RECONNECT_ERROR, server->address);
		fe_net_disconnect(server->net);
		server->net = NULL;
		return(-1);
	}
	irc_traverse_channel_list(server->channels, (traverse_t) irc_server_rejoin_channel, server);
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
 * error).  The message is destroyed by this function and must not be
 * referenced after calling this function.
 */
int irc_send_msg(struct irc_server *server, struct irc_msg *msg)
{
	int size, ret;
	char buffer[IRC_MAX_MSG];

	if (!(server->bitflags & IRC_SBF_CONNECTED)) {
		queue_append_node_v(server->send_queue, msg, queue);
	}
	else {
		if (server && msg && (size = irc_marshal_msg(msg, buffer, IRC_MAX_MSG)))
			ret = fe_net_send(server->net, buffer, size);
		// TODO should we dispatch messages here instead?
		irc_destroy_msg(msg);
		if (ret != size)
			return(-1);
	}
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
		if ((size = fe_net_receive_str(server->net, buffer, IRC_MAX_MSG + 1, '\n')) < 0) {
			if ((server->bitflags & IRC_SBF_RECONNECTING) || irc_server_reconnect(server))
				return(NULL);
			server->bitflags |= IRC_SBF_RECONNECTING;
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

	if (!(msg = irc_unmarshal_msg(buffer)))
		return(NULL);
	msg->server = server;
	return(msg);
}

/**
 * Send the given message to every server in the server list and return 0
 * on success or -1 if any particular send fails.  If a send fails, the
 * function will continue attempting to send messages.  The message is
 * destroyed by this function and must not be referenced after calling this
 * function.
 */
int irc_broadcast_msg(struct irc_msg *msg)
{
	int ret = 0;
	struct irc_msg *new_msg;

	linear_traverse_list_v(server_list, sl,
		if (new_msg = irc_duplicate_msg(msg))
			ret = irc_send_msg(&cur->server, new_msg);
	);
	irc_destroy_msg(msg);
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

	if (!(msg = irc_create_msg(IRC_MSG_JOIN, NULL, NULL, 1, name)))
		return(-1);
	irc_send_msg(server, msg);
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
	return(0);
}

/**
 * Change the nick in the given server to the given nick and return 0
 * on success or -1 on failure.
 */
int irc_change_nick(struct irc_server *server, char *nick)
{
	int ret;
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_NICK, NULL, NULL, 1, nick)))
		return(-1);
	ret = irc_send_msg(server, msg);
	if (!ret) {
		strncpy(server->nick, nick, IRC_MAX_NICK - 1);
		server->nick[IRC_MAX_NICK - 1] = '\0';
	}
	return(ret);
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
	signal_emit("irc_dispatch_msg", NULL, msg);
	return(irc_send_msg(server, msg));
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
	signal_emit("irc_dispatch_msg", NULL, msg);
	return(irc_send_msg(server, msg));
}

/*** Local Functions ***/

/**
 * Initialize communications with the server and return 0 on success
 * or -1 on error.
 */
static int irc_server_init_connection(struct irc_server *server)
{
	int ret = 0;
	struct irc_msg *msg;

	if (!(server->net = fe_net_connect(server->address, server->port, (callback_t) irc_server_receive, server)))
		return(-1);

	server->bitflags |= IRC_SBF_CONNECTED;
	if ((msg = irc_create_msg(IRC_MSG_NICK, NULL, NULL, 1, server->nick)) && !irc_send_msg(server, msg)
	    && (msg = irc_create_msg(IRC_MSG_USER, NULL, NULL, 4, server->nick, "0", "0", "Person Pants")) && !irc_send_msg(server, msg)) {
		server->bitflags &= ~IRC_SBF_CONNECTED;
		return(0);
	}
	fe_net_disconnect(server->net);
	server->bitflags &= ~IRC_SBF_CONNECTED;
	server->net = NULL;
	return(-1);
}

/**
 * Called by network when data is available from the given socket for
 * the given server.
 */
static int irc_server_receive(struct irc_server *server, network_t net)
{
	struct irc_msg *msg;

	if (msg = irc_receive_msg(server))
		signal_emit("irc_dispatch_msg", NULL, msg);
	irc_destroy_msg(msg);
	if (server->bitflags & IRC_SBF_CONNECTED)
		irc_server_flush_send_queue(server);
	return(0);
}

/**
 * Join all the channels after reconnecting to the server.
 */
static int irc_server_rejoin_channel(struct irc_channel *channel, struct irc_server *server)
{
	if (strcmp(channel->name, IRC_SERVER_STATUS_CHANNEL))
		return(irc_join_channel(server, channel->name));
	return(0);
}

/**
 * 
 */
static int irc_server_flush_send_queue(struct irc_server *server)
{
	queue_destroy_v(server->send_queue, queue,
		irc_send_msg(server, cur);
	);
	return(0);
}


