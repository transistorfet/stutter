/*
 * Module Name:		server.c
 * Description:		Server Interface Manager
 */

#include <string.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/string.h>
#include <stutter/memory.h>
#include <stutter/signal.h>
#include <stutter/macros.h>
#include <stutter/object.h>
#include <stutter/output.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/frontend/net.h>
#include <stutter/frontend/timer.h>
#include <stutter/modules/irc/irc.h>
#include <stutter/modules/irc/msg.h>
#include <stutter/modules/irc/server.h>
#include <stutter/modules/irc/channel.h>

#define SERVER_IS_UNBUFFERABLE_MSG(msg)		((msg->cmd == IRC_MSG_NICK) || (msg->cmd == IRC_MSG_USER) || (msg->cmd == IRC_MSG_PASS) || (msg->cmd == IRC_MSG_OPER) || (msg->cmd == IRC_MSG_QUIT) || (msg->cmd == IRC_MSG_SQUIT))

static int server_initialized = 0;
static fe_timer_t irc_ping_watchdog_timer;
static struct irc_server *server_list = NULL;

static int irc_server_init_connection(struct irc_server *);
static int irc_server_auto_reconnect(struct irc_server *, fe_timer_t);
static int irc_server_receive(struct irc_server *, fe_network_t, fe_network_t);
static int irc_server_rejoin_channels(struct irc_server *);
static int irc_server_flush_send_queue(struct irc_server *);
static int irc_server_ping_watchdog(void *, fe_timer_t);

struct variable_type_s irc_server_type = { {
	OBJECT_TYPE_S(&variable_type),
	"irc_server",
	sizeof(struct irc_server),
	NULL,
	(object_init_t) irc_server_init,
	(object_release_t) irc_server_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) NULL,
	(variable_evaluate_t) base_command_evaluate
};

int init_irc_server(void)
{
	if (server_initialized)
		return(0);
	irc_ping_watchdog_timer = fe_timer_create(FE_TIMER_BF_PERIODIC, 60, (callback_t) irc_server_ping_watchdog, NULL);
	server_initialized = 1;
	return(0);
}

int release_irc_server(void)
{
	struct irc_server *tmp, *cur;

	if (irc_ping_watchdog_timer)
		fe_timer_destroy(irc_ping_watchdog_timer);
	for (cur = server_list; cur; cur = tmp) {
		tmp = cur->next;
		destroy_object(OBJECT_S(cur));
	}
	server_initialized = 0;
	return(0);
}


int irc_server_init(struct irc_server *server, const char *params, va_list va)
{
	char *nick = NULL;

	server->address = NULL;
	server->port = 0;
	if (!strcmp(params, "s"))
		nick = va_arg(va, char *);
	server->nick = duplicate_string(nick ? nick : IRC_DEFAULT_NICK);
	if (!(server->send_queue = create_queue(0, (destroy_t) destroy_object)))
		return(-1);
	server->status = irc_add_channel(&server->channels, IRC_SERVER_STATUS_CHANNEL, server);
	server->next = server_list;
	server_list = server;
	return(0);
}

void irc_server_release(struct irc_server *server)
{
	struct irc_server *cur, *prev;

	for (cur = server_list, prev = NULL; cur; prev = cur, cur = cur->next) {
		if (cur == server) {
			if (prev)
				prev->next = cur->next;
			else
				server_list = cur->next;
			break;
		}
	}
	irc_server_disconnect(server);
	irc_destroy_channel_list(&server->channels);
	if (server->send_queue)
		destroy_queue(server->send_queue);
	if (server->address)
		destroy_string(server->address);
	if (server->nick)
		destroy_string(server->nick);
}








/*
 * Connect the given server structure to the address:port given.
 */
int irc_server_connect(struct irc_server *server, const char *address, int port)
{
	if (server->address)
		destroy_string(server->address);
	server->address = create_string(address);
	server->port = port;

	if (irc_server_init_connection(server) < 0) {
		irc_server_disconnect(server);
		return(-1);
	}
	return(0);
}

/**
 * Reestablish a connection to the given server
 */
int irc_server_reconnect(struct irc_server *server)
{
	if (server->net)
		irc_server_disconnect(server);
	server->bitflags &= ~IRC_SBF_CONNECTED;
	if (server->send_queue)
		queue_clear(server->send_queue);

	server->attempts++;
	if (irc_server_init_connection(server)) {
		OUTPUT_ERROR(IRC_ERR_RECONNECT_ERROR, server->address);
		irc_server_disconnect(server);
		return(-1);
	}
	server->attempts = 0;
	irc_server_rejoin_channels(server);
	return(0);
}

/**
 * Disconnect and destroy the server structure given.
 */
int irc_server_disconnect(struct irc_server *server)
{
	if (server->net) {
		fe_net_disconnect(server->net);
		server->net = NULL;
	}
	server->bitflags &= ~IRC_SBF_CONNECTED;
	if (server->send_queue)
		queue_clear(server->send_queue);
	return(0);
}

/**
 * Return the server struct for the given address or return NULL
 * if not connected to that address.
 */
struct irc_server *irc_find_server(const char *address)
{
	struct irc_server *cur;

	for (cur = server_list; cur; cur = cur->next) {
		if (!strcmp(cur->address, address))
			return(cur);
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

	if (!(server->bitflags & IRC_SBF_CONNECTED) && !SERVER_IS_UNBUFFERABLE_MSG(msg))
		queue_append(server->send_queue, msg);
	else {
		if (server && server->net && msg && (size = irc_marshal_msg(msg, buffer, IRC_MAX_MSG)))
			ret = fe_net_send(server->net, buffer, size);
		if ((msg->cmd == IRC_MSG_PRIVMSG) || (msg->cmd == IRC_MSG_NOTICE))
			irc_dispatch_msg(msg);
		destroy_object(OBJECT_S(msg));
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
		if ((size = fe_net_receive_text(server->net, buffer, IRC_MAX_MSG + 1, '\n')) < 0) {
			OUTPUT_ERROR(IRC_ERR_SERVER_DISCONNECTED, server->address);
			if (IRC_RECONNECT_RETRIES && (server->attempts > IRC_RECONNECT_RETRIES))
				return(NULL);
			irc_server_disconnect(server);
			if (fe_timer_create(FE_TIMER_BF_PERIODIC, IRC_RETRY_DELAY, (callback_t) irc_server_auto_reconnect, server))
				OUTPUT_STATUS(IRC_OUT_ATTEMPTING_RECONNECT, IRC_RETRY_DELAY);
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
	struct irc_server *cur;
	struct irc_msg *new_msg;

	for (cur = server_list; cur; cur = cur->next) {
		if ((new_msg = irc_duplicate_msg(msg)))
			ret = irc_send_msg(cur, new_msg);
		cur->last_ping = time(NULL);
	}
	destroy_object(OBJECT_S(msg));
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
int irc_join_channel(struct irc_server *server, const char *name)
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
int irc_leave_channel(struct irc_server *server, const char *name)
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
int irc_change_nick(struct irc_server *server, const char *nick)
{
	struct irc_msg *msg;

	if (server->net) {
		if (!(msg = irc_create_msg(IRC_MSG_NICK, NULL, NULL, 1, 0, nick)))
			return(-1);
		return(irc_send_msg(server, msg));
	}
	else {
		if (server->nick)
			destroy_string(server->nick);
		server->nick = create_string(nick);
	}
	return(0);
}

/**
 * Send the given string to the given channel or nick on the given
 * server and return -1 if the send fail and 0 on success.
 */
int irc_private_msg(struct irc_server *server, const char *name, const char *text)
{
	char ch;
	int i = 0, j = 0;
	struct irc_msg *msg;
	int name_len, text_len, ret;

	name_len = strlen(name);
	text_len = strlen(text);

	// TODO fix this so that you don't modify the const char text string
	if (!(msg = irc_create_msg(IRC_MSG_PRIVMSG, NULL, NULL, 2, 0, name, text)))
		return(-1);
	msg->server = server;
	if ((ret = irc_send_msg(server, msg) < 0))
		return(ret);
/*
	do {
		i = j;
		j = ((j + 488 - name_len) < text_len) ? (j + 488 - name_len) : text_len;
		ch = text[j];
		text[j] = '\0';
		if (!(msg = irc_create_msg(IRC_MSG_PRIVMSG, NULL, NULL, 2, 0, name, &text[i])))
			return(-1);
		text[j] = ch;
		msg->server = server;
		if ((ret = irc_send_msg(server, msg) < 0))
			return(ret);
	} while (j < text_len);
*/
	return(0);
}

/**
 * Send the given string to the given channel or nick on the given
 * server as a notice message and return -1 if the send fail and 0 on
 * success.
 */
int irc_notice(struct irc_server *server, const char *name, const char *text)
{
	char ch;
	int i = 0, j = 0;
	struct irc_msg *msg;
	int name_len, text_len, ret;

	name_len = strlen(name);
	text_len = strlen(text);

	// TODO fix this so that you don't modify the const char text string
	if (!(msg = irc_create_msg(IRC_MSG_NOTICE, NULL, NULL, 2, 0, name, &text[i])))
		return(-1);
	msg->server = server;
	if ((ret = irc_send_msg(server, msg) < 0))
		return(ret);
/*
	do {
		i = j;
		j = ((j + 488 - name_len) < text_len) ? (j + 488 - name_len) : text_len;
		ch = text[j];
		text[j] = '\0';
		if (!(msg = irc_create_msg(IRC_MSG_NOTICE, NULL, NULL, 2, 0, name, &text[i])))
			return(-1);
		msg->server = server;
		if ((ret = irc_send_msg(server, msg)))
			return(ret);
	} while (j < text_len);
*/
	return(0);
}

/**
 * Send the given ctcp message type with the given arguments to the given
 * channel or nick on the given server and return -1 if the send fail and
 * 0 on success.
 */
int irc_ctcp_msg(struct irc_server *server, const char *cmd, const char *name, const char *text)
{
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_PRIVMSG, NULL, NULL, 2, 1, name, "", cmd, text)))
		return(-1);
	msg->server = server;
	return(irc_send_msg(server, msg));
}

/**
 * Send the given ctcp message type with the given arguments to the given
 * channel or nick on the given server and return -1 if the send fail and
 * 0 on success.
 */
int irc_ctcp_reply(struct irc_server *server, const char *cmd, const char *name, const char *text)
{
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_NOTICE, NULL, NULL, 2, 1, name, "", cmd, text)))
		return(-1);
	msg->server = server;
	return(irc_send_msg(server, msg));
}

/*** Local Functions ***/

/**
 * Initialize communications with the server and return 0 on success
 * or -1 on error.
 */
static int irc_server_init_connection(struct irc_server *server)
{
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

	if (server->bitflags & IRC_SBF_CONNECTED) {
		fe_timer_destroy(timer);
		return(0);
	}
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

	if ((msg = irc_receive_msg(server))) {
		irc_dispatch_msg(msg);
		destroy_object(OBJECT_S(msg));
	}
	if (server->bitflags & IRC_SBF_CONNECTED)
		irc_server_flush_send_queue(server);
	return(0);
}

/**
 * Join all the channels after reconnecting to the server.
 */
static int irc_server_rejoin_channels(struct irc_server *server)
{
	struct irc_channel *cur;

	for (cur = server->channels.head; cur; cur = cur->next) {
		/** Reconnect to all channels except the status virtual channel */
		if (strcmp_icase(cur->name, IRC_SERVER_STATUS_CHANNEL))
			return(irc_join_channel(server, cur->name));
	}
	return(0);
}

/**
 * Send all messages currently held in the send queue.
 */
static int irc_server_flush_send_queue(struct irc_server *server)
{
	QUEUE_FOREACH(server->send_queue)
		irc_send_msg(server, QUEUE_CURRENT(server->send_queue));
	queue_clear(server->send_queue);
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
	struct irc_server *cur;

	current_time = time(NULL);
	for (cur = server_list; cur; cur = cur->next) {
		if ((current_time - cur->last_ping) >= IRC_PING_WATCHDOG_TIMEOUT) {
			if (cur->bitflags & IRC_SBF_CONNECTED) {
				OUTPUT_ERROR(IRC_ERR_SERVER_DISCONNECTED, cur->address);
			}
			if (!IRC_RECONNECT_RETRIES || (cur->attempts <= IRC_RECONNECT_RETRIES))
				irc_server_reconnect(cur);
		}
	}
	return(0);
}


