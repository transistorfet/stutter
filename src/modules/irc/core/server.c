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

struct callback_s *irc_server_dispatch;

static int irc_server_receive(struct irc_server *, network_t);
static int server_init_connection(struct irc_server *);

/**
 * Create a new server structure and connect/initialize it to the
 * address:port given.
 */
struct irc_server *irc_server_connect(char *address, int port, char *nick)
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

	server->status = irc_create_channel(IRC_SERVER_STATUS_CHANNEL, NULL, server);
	server->channels = irc_create_channel_list();
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
	destroy_list(server->channels);
	irc_destroy_channel(server->status);
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

int irc_dispatch_msg(struct irc_server *server, struct irc_msg *msg)
{
	return(execute_callback(irc_server_dispatch, msg));
}


/**
 * Send the join command to the given server in order to join the channel
 * on the server with the given name, create a channel structure for the
 * channel and add it to the server channel list.  A -1 is returned if
 * the send fails or a 0 on success.  If the channel already exits, the
 * join command will still be sent but a new channel structure will not
 * be created.
 */
struct irc_channel *irc_join_channel(struct irc_server *server, char *name, void *ptr)
{
	struct irc_msg *msg;
	struct irc_channel *channel;

	if (msg = irc_create_msg(IRC_MSG_JOIN, NULL, NULL, 1, name)) {
		if (!irc_send_msg(server, msg)) {
			if ((channel = irc_get_channel(server, name)) || ((channel = irc_create_channel(name, ptr, server)) && !list_add(server->channels, channel)))
				return(channel);
			irc_destroy_channel(channel);
		}
		irc_destroy_msg(msg);
	}
	return(NULL);
/*
	if (irc_get_channel(server, name))
		return(NULL);
	if (!(msg = irc_create_msg(IRC_MSG_JOIN, NULL, NULL, 1, name)))
		return(NULL);
	if (!(channel = irc_create_channel(name, ptr, server)))
		return(NULL);
	if (irc_send_msg(server, msg))
		return(NULL);
	irc_destroy_msg(msg);
	list_add(server->channels, channel);
	return(channel);
*/
}

/**
 * Leave the channel with the given name on the given server, remove the
 * appropriate channel structure from the server channel list and destroy
 * that structure.  A -1 is returned if the connection fails or a 0 on
 * success or if the channel does not exist in the list.
 */
int irc_leave_channel(struct irc_server *server, char *name)
{
	struct irc_msg *msg;
	struct irc_channel *channel;

	if (!(msg = irc_create_msg(IRC_MSG_PART, NULL, NULL, 1, name)))
		return(-1);
	if (!(channel = list_find(server->channels, name, 0)))
		return(-2);
	if (irc_send_msg(server, msg))
		return(-3);
	irc_destroy_msg(msg);
	list_delete(server->channels, name);
	return(0);
}

/**
 * Change the nick in the given server to the given nick and return 0
 * on success or -1 on failure.
 */
int irc_change_nick(struct irc_server *server, char *nick)
{
	struct irc_msg *msg;
	struct irc_user *user;
	struct irc_channel *channel;

	if (!(msg = irc_create_msg(IRC_MSG_NICK, NULL, NULL, 1, nick)))
		return(-1);
	if (irc_send_msg(server, msg))
		return(-1);
	irc_destroy_msg(msg);

// TODO this changes the nick of everyone but a message should be sent back to us in which
//	case our message receiver will do this for us (as if it was the nickchange of
//	someone else.

//	list_clear_current(server->channels);
//	while (channel = list_next(server->channels)) {
//		if (user = irc_get_user(channel, server->nick))
//			irc_change_user_nick(user, nick);
//	}
	strncpy(server->nick, nick, IRC_MAX_NICK - 1);
	return(0);
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
		execute_callback(irc_server_dispatch, msg);
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

	if (!(msg = irc_create_msg(IRC_MSG_USER, NULL, NULL, 4, server->nick, "0", "0", "IRCer")))
		return(-1);
	ret = irc_send_msg(server, msg);
	irc_destroy_msg(msg);
	if (ret)
		return(-1);
	return(0);
}



