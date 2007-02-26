/*
 * Module Name:		server.c
 * Version:		0.1
 * Module Requirements:	frontend ; queue ; memory ; linear ; msg ; channel
 * Description:		Server Interface Manager
 */


#include <string.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/memory.h>
#include <stutter/linear.h>
#include <stutter/signal.h>
#include <stutter/globals.h>
#include <stutter/frontend/net.h>
#include <stutter/frontend/timer.h>
#include <stutter/modules/irc/msg.h>
#include <stutter/modules/irc/server.h>
#include <stutter/modules/irc/channel.h>

#define server_is_unbufferable_msg_m(msg)		((msg->cmd == IRC_MSG_NICK) || (msg->cmd == IRC_MSG_USER) || (msg->cmd == IRC_MSG_PASS) || (msg->cmd == IRC_MSG_OPER) || (msg->cmd == IRC_MSG_QUIT) || (msg->cmd == IRC_MSG_SQUIT))

struct irc_server_node {
	struct irc_server server;
	linear_node_v(irc_server_node) sl;
};

static int server_initialized = 0;
static fe_timer_t irc_ping_watchdog_timer;
static linear_list_v(irc_server_node) server_list;

static int irc_server_init_connection(struct irc_server *);
static int irc_server_auto_reconnect(struct irc_server *, fe_timer_t);
static int irc_server_receive(struct irc_server *, fe_network_t, fe_network_t);
static int irc_server_rejoin_channel(struct irc_channel *, struct irc_server *);
static int irc_server_flush_send_queue(struct irc_server *);
static int irc_server_ping_watchdog(void *, fe_timer_t);

int init_irc_server(void)
{
	if (server_initialized)
		return(0);
	linear_init_v(server_list);
	add_signal("irc.dispatch_msg", SIG_BF_USE_WILDCARD_INDEX);
	add_signal("irc.dispatch_ctcp", SIG_BF_USE_WILDCARD_INDEX);
	irc_ping_watchdog_timer = fe_timer_create(FE_TIMER_BF_PERIODIC, 60, irc_server_ping_watchdog, NULL);
	server_initialized = 1;
	return(0);
}

int release_irc_server(void)
{
	struct irc_server_node *tmp, *cur;

	if (irc_ping_watchdog_timer)
		fe_timer_destroy(irc_ping_watchdog_timer);
	remove_signal("irc.dispatch_msg");
	remove_signal("irc.dispatch_ctcp");
	linear_foreach_safe_v(server_list, sl, cur, tmp) {
		if (cur->server.channels)
			irc_destroy_channel_list(cur->server.channels);
		memory_free(cur);
	}
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

	node->server.address = (char *) offset_after_struct_m(node, 0);
	strcpy(node->server.address, address);
	node->server.port = port;
	strncpy(node->server.nick, nick, IRC_MAX_NICK);
	queue_init_v(node->server.send_queue, 0);

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
	struct irc_msg *cur, *tmp;

	if (server->net)
		fe_net_disconnect(server->net);
	server->bitflags &= ~IRC_SBF_CONNECTED;
	queue_foreach_safe_v(server->send_queue, queue, cur, tmp) {
		irc_destroy_msg(cur);
	}
	queue_release_v(server->send_queue);

	server->attempts++;
	if (irc_server_init_connection(server)) {
		IRC_ERROR_JOINPOINT(IRC_ERR_RECONNECT_ERROR, server->address)
		fe_net_disconnect(server->net);
		server->net = NULL;
		return(-1);
	}
	server->attempts = 0;
	irc_traverse_channel_list(server->channels, (traverse_t) irc_server_rejoin_channel, server);
	return(0);
}

/**
 * Disconnect and destroy the server structure given.
 */
int irc_server_disconnect(struct irc_server *server)
{
	struct irc_msg *cur, *tmp;

	linear_remove_node_v(server_list, sl, (struct irc_server_node *) server);
	irc_destroy_channel_list(server->channels);
	fe_net_disconnect(server->net);
	queue_foreach_safe_v(server->send_queue, queue, cur, tmp) {
		irc_destroy_msg(cur);
	}
	memory_free(server);
	return(0);
}

/**
 * Return the server struct for the given address or return NULL
 * if not connected to that address.
 */
struct irc_server *irc_find_server(char *address)
{
	struct irc_server_node *cur;

	linear_foreach_v(server_list, sl, cur) {
		if (!strcmp(cur->server.address, address))
			return(&cur->server);
	}
	return(NULL);
}

/**
 * Return the channel that has the given window handle out of all the channels
 * on all the servers.  If no channel is associated with the given window
 * then NULL is returned.
 */
struct irc_channel *irc_server_find_window(void *window)
{
	struct irc_channel *channel;
	struct irc_server_node *cur;

	linear_foreach_v(server_list, sl, cur) {
		if (channel = irc_channel_find_window(cur->server.channels, window))
			return(channel);
	}
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

	if (!(server->bitflags & IRC_SBF_CONNECTED) && !server_is_unbufferable_msg_m(msg)) {
		queue_append_node_v(server->send_queue, queue, msg);
	}
	else {
		if (server && server->net && msg && (size = irc_marshal_msg(msg, buffer, IRC_MAX_MSG)))
			ret = fe_net_send(server->net, buffer, size);
		// TODO should we dispatch messages here instead?
		irc_destroy_msg(msg);
		server->last_ping = time(NULL);
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
			IRC_ERROR_JOINPOINT(IRC_ERR_SERVER_DISCONNECTED, server->address)
			if (IRC_RECONNECT_RETRIES && (server->attempts > IRC_RECONNECT_RETRIES))
				return(NULL);
			fe_net_disconnect(server->net);
			server->net = NULL;
			if (fe_timer_create(FE_TIMER_BF_PERIODIC, IRC_RETRY_DELAY, (callback_t) irc_server_auto_reconnect, server)) {
				IRC_OUTPUT_JOINPOINT(IRC_OUT_ATTEMPTING_RECONNECT, IRC_RETRY_DELAY)
			}
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

	if (!(msg = irc_unmarshal_msg(buffer)))
		return(NULL);
	msg->server = server;
	server->last_ping = time(NULL);
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
	struct irc_server_node *cur;

	linear_foreach_v(server_list, sl, cur) {
		if (new_msg = irc_duplicate_msg(msg))
			ret = irc_send_msg(&cur->server, new_msg);
		cur->server.last_ping = time(NULL);
	}
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

	if (!(msg = irc_create_msg(IRC_MSG_JOIN, NULL, NULL, 1, 0, name)))
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

	if (!(msg = irc_create_msg(IRC_MSG_PART, NULL, NULL, 1, 0, name)))
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
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_NICK, NULL, NULL, 1, 0, nick)))
		return(-1);
	return(irc_send_msg(server, msg));
}

/**
 * Send the given string to the given channel or nick on the given
 * server and return -1 if the send fail and 0 on success.
 */
int irc_private_msg(struct irc_server *server, char *name, char *text)
{
	char ch;
	int i = 0, j = 0;
	struct irc_msg *msg;
	int name_len, text_len, ret;

	name_len = strlen(name);
	text_len = strlen(text);
	do {
		i = j;
		j = ((j + 488 - name_len) < text_len) ? (j + 488 - name_len) : text_len;
		ch = text[j];
		text[j] = '\0';
		if (!(msg = irc_create_msg(IRC_MSG_PRIVMSG, NULL, NULL, 2, 0, name, &text[i])))
			return(-1);
		text[j] = ch;
		msg->server = server;
		emit_signal(NULL, "irc.dispatch_msg", msg);
		if (ret = irc_send_msg(server, msg) < 0)
			return(ret);
	} while (j < text_len);
	return(0);
}

/**
 * Send the given string to the given channel or nick on the given
 * server as a notice message and return -1 if the send fail and 0 on
 * success.
 */
int irc_notice(struct irc_server *server, char *name, char *text)
{
	char ch;
	int i = 0, j = 0;
	struct irc_msg *msg;
	int name_len, text_len, ret;

	name_len = strlen(name);
	text_len = strlen(text);
	do {
		i = j;
		j = ((j + 488 - name_len) < text_len) ? (j + 488 - name_len) : text_len;
		ch = text[j];
		text[j] = '\0';
		if (!(msg = irc_create_msg(IRC_MSG_NOTICE, NULL, NULL, 2, 0, name, &text[i])))
			return(-1);
		msg->server = server;
		emit_signal(NULL, "irc.dispatch_msg", msg);
		if (ret = irc_send_msg(server, msg))
			return(ret);
	} while (j < text_len);
	return(0);
}

/**
 * Send the given ctcp message type with the given arguments to the given
 * channel or nick on the given server and return -1 if the send fail and
 * 0 on success.
 */
int irc_ctcp_msg(struct irc_server *server, char *cmd, char *name, char *text)
{
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_PRIVMSG, NULL, NULL, 2, 1, name, "", cmd, text)))
		return(-1);
	msg->server = server;
	emit_signal(NULL, "irc.dispatch_msg", msg);
	return(irc_send_msg(server, msg));
}

/**
 * Send the given ctcp message type with the given arguments to the given
 * channel or nick on the given server and return -1 if the send fail and
 * 0 on success.
 */
int irc_ctcp_reply(struct irc_server *server, char *cmd, char *name, char *text)
{
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_NOTICE, NULL, NULL, 2, 1, name, "", cmd, text)))
		return(-1);
	msg->server = server;
	emit_signal(NULL, "irc.dispatch_msg", msg);
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

	if (!(server->net = fe_net_connect(server->address, server->port)))
		return(-1);
	fe_net_set_callback(server->net, IO_COND_READ, (callback_t) irc_server_receive, server);

	if ((msg = irc_create_msg(IRC_MSG_NICK, NULL, NULL, 1, 0, server->nick)) && !irc_send_msg(server, msg)
	    && (msg = irc_create_msg(IRC_MSG_USER, NULL, NULL, 4, 0, server->nick, "0", "0", "Person Pants")) && !irc_send_msg(server, msg)) {
		server->last_ping = time(NULL);
		return(0);
	}
	fe_net_disconnect(server->net);
	server->bitflags &= ~IRC_SBF_CONNECTED;
	server->net = NULL;
	return(-1);
}

/**
 * Reconnect to the given server after the retry delay has expired.  If the
 * attempt fails, the timer is reset unless the maximum number of retries has
 * been reached.
 */
static int irc_server_auto_reconnect(struct irc_server *server, fe_timer_t timer)
{
	int ret;

	ret = irc_server_reconnect(server);
	if (!ret || !IRC_RECONNECT_RETRIES || (server->attempts <= IRC_RECONNECT_RETRIES))
		fe_timer_destroy(timer);
	return(ret);
}

/**
 * Called by network when data is available from the given socket for
 * the given server.
 */
static int irc_server_receive(struct irc_server *server, fe_network_t desc, fe_network_t net)
{
	struct irc_msg *msg;

	if (msg = irc_receive_msg(server))
		emit_signal(NULL, "irc.dispatch_msg", msg);
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
 * Send all messages currently held in the send queue.
 */
static int irc_server_flush_send_queue(struct irc_server *server)
{
	struct irc_msg *cur, *tmp;

	queue_foreach_safe_v(server->send_queue, queue, cur, tmp) {
		irc_send_msg(server, cur);
	}
	queue_release_v(server->send_queue);
	return(0);
}

/**
 * Called periodically to check for a ping timeout.  If the last_ping time
 * is greater than IRC_PING_WATCHDOG_TIMEOUT then the server is reconnected
 * to.
 */
static int irc_server_ping_watchdog(void *ptr, fe_timer_t timer)
{
	time_t current_time;
	struct irc_server_node *cur;

	current_time = time(NULL);
	linear_foreach_v(server_list, sl, cur) {
		if ((current_time - cur->server.last_ping) >= IRC_PING_WATCHDOG_TIMEOUT) {
			if (cur->server.bitflags & IRC_SBF_CONNECTED) {
				IRC_ERROR_JOINPOINT(IRC_ERR_SERVER_DISCONNECTED, cur->server.address)
			}
			if (!IRC_RECONNECT_RETRIES || (cur->server.attempts <= IRC_RECONNECT_RETRIES))
				irc_server_reconnect(&cur->server);
		}
	}
	return(0);
}


