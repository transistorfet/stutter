/*
 * Module Name:		server.c
 * Version:		0.1
 * Module Requirements:	msg ; channel ; net ; list
 * Description:		Server Interface Manager
 */


#include <string.h>
#include <stdlib.h>

#include CONFIG_H
#include <nit/net.h>
#include <nit/list.h>
#include <nit/callback.h>
#include <modules/irc/msg.h>
#include <modules/irc/server.h>
#include <modules/irc/channel.h>

static int irc_server_receive(struct irc_server *, network_t);
static int server_init_connection(struct irc_server *);

/**
 * Create a new server structure and connect/initialize it to the
 * address:port given.
 */
struct irc_server *irc_server_connect(char *address, int port, char *nick, void *window)
{
	struct irc_server *server;

	if (!(server = (struct irc_server *) malloc(sizeof(struct irc_server) + strlen(address) + 1)))
		return(NULL);
	memset(server, '\0', sizeof(struct irc_server));
	if (!(server->net = net_connect(address, port))) {
		free(server);
		return(NULL);
	}
	server->address = (char *) (((size_t) server) + sizeof(struct irc_server));
	strcpy(server->address, address);
	server->port = port;
	strncpy(server->nick, nick, IRC_MAX_NICK);

	server->channels = irc_create_channel_list();
	server->status = irc_add_channel(server->channels, IRC_SERVER_STATUS_CHANNEL, window, server);
	net_receive_callback(server->net, create_callback(0, 0, NULL, (callback_t) irc_server_receive, server));

	if (server_init_connection(server)) {
		irc_server_disconnect(server);
		return(NULL);
	}
	return(server);
}

/**
 * Reestablish a connection to the given server
 */
int irc_server_reconnect(struct irc_server *server)
{
	net_disconnect(server->net);
	server->net = net_connect(server->address, server->port);
	// TODO register network callback
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
	irc_destroy_channel_list(server->channels);
	net_disconnect(server->net);
	free(server);
	return(0);
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
	if (net_send(server->net, buffer, size) != size)
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
	char buffer[IRC_MAX_MSG + 1];

	if (!server || !server->net)
		return(NULL);

	while (1) {
		if ((size = net_receive(server->net, buffer, IRC_MAX_MSG + 1)) < 0) {
			net_disconnect(server->net);
			server->net = 0;
			return(NULL);
		}
		else if (size == 0)
			return(NULL);
		else if (!strncmp(buffer, "PING", 4)) {
			buffer[1] = 'O';
			if (net_send(server->net, buffer, size) != size)
				return(NULL);
			server->last_ping = time(NULL);
		}
		else
			break;
	}

	return(irc_parse_msg(buffer));
}

/**
 * Send the join command to the given server in order to join the channel
 * on the server with the given name, create a channel structure for the
 * channel and add it to the server channel list.  NULL is returned if
 * the send fails or a pointer to the channel struct on success.  If the
 * channel already exits, the join command will still be sent but a new
 * channel structure will not be created.
 */
struct irc_channel *irc_join_channel(struct irc_server *server, char *name, void *window)
{
	struct irc_msg *msg;
	struct irc_channel *channel;

	if (msg = irc_create_msg(IRC_MSG_JOIN, NULL, NULL, 1, name)) {
		// TODO can we put this find_channel call into the add_channel call and avoid this indirection
		// 	the join command already checks for the channel so that it doesn't create an unused window
		//	but not all functions calling join will do this
		if (channel = irc_find_channel(server->channels, name))
			irc_send_msg(server, msg);
		else if (channel = irc_add_channel(server->channels, name, window, server)) {
			if (irc_send_msg(server, msg)) {
				irc_remove_channel(server->channels, name);
				channel = NULL;
			}
		}
		irc_destroy_msg(msg);
		return(channel);
	}
	return(NULL);
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

	if (msg = irc_create_msg(IRC_MSG_PART, NULL, NULL, 1, name)) {
		if (!irc_send_msg(server, msg))
			irc_remove_channel(server->channels, name);
		irc_destroy_msg(msg);
		return(0);
	}
	return(-1);
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
	if (!irc_send_msg(server, msg))
		strncpy(server->nick, nick, IRC_MAX_NICK - 1);
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
	if (!(ret = irc_send_msg(server, msg)))
		irc_dispatch_msg(server, msg);
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
	if (!(ret = irc_send_msg(server, msg)))
		irc_dispatch_msg(server, msg);
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
		irc_dispatch_msg(server, msg);
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

	if (!(msg = irc_create_msg(IRC_MSG_NICK, NULL, NULL, 1, server->nick)))
		return(-1);
	ret = irc_send_msg(server, msg);
	irc_destroy_msg(msg);
	if (ret)
		return(-1);

	if (!(msg = irc_create_msg(IRC_MSG_USER, NULL, NULL, 4, server->nick, "0", "0", "Person Pants")))
		return(-1);
	ret = irc_send_msg(server, msg);
	irc_destroy_msg(msg);
	if (ret)
		return(-1);
	return(0);
}



