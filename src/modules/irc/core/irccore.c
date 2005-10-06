/*
 * Module Name:		irccore.c
 * Version:		0.1
 * Module Requirements:	msg ; user ; server ; channel ; list ; memory ; callback
 * Description:		IRC Interface Manager
 */


#include <string.h>

#include <frontend.h>
#include <nit/list.h>
#include <nit/memory.h>
#include <nit/callback.h>
#include <modules/irc/msg.h>
#include <modules/irc/user.h>
#include <modules/irc/server.h>
#include <modules/irc/channel.h>
#include <modules/irc/irccore.h>

struct irc_s *irc;

/**
 * Initialize the irc module and return 0 if successful or -1 on error.
 */
int init_irc(void)
{
	if (irc)
		return(-1);

	if (!(irc = (struct irc_s *) memory_alloc(sizeof(struct irc_s))))
		return(-1);
	memset(irc, '\0', sizeof(struct irc_s));

	if (!(irc->servers = irc_create_server_list())) {
		release_irc();
		return(-1);
	}

	return(0);
}

/**
 * Shut down the irc module and release all resources.
 */
int release_irc(void)
{
	if (!irc)
		return(-1);
	if (irc->servers)
		destroy_list(irc->servers);
//	if (irc->dispatch)
//		destroy_callback(irc->dispatch);
	memory_free(irc);
	irc = NULL;
	return(0);
}

/**
 * Connect to the irc server specified by the given address and port number,
 * set the nick as given and direct the status window to the given window.
 */
int irc_connect(char *address, int port, char *nick, void *window)
{
	struct irc_server *server;

	if (!(server = irc_server_connect(address, port, nick)))
		return(-1);
	server->status->window = window;
	list_add(irc->servers, server);
	irc->current = server->status;
	return(0);
}

/**
 * Disconnect from the given server and remove it from the server list.
 * All channels are destroyed.  The windows associated with those channels
 * are not destroyed and must be freed seperately.
 */
int irc_disconnect(struct irc_server *server)
{
	// TODO is there more than this?
	list_delete(irc->servers, server);
	return(0);
}

/**
 * Send the given message to every server in the server list and return 0
 * on success or -1 if any particular send fails.  If a send fails, the
 * function will continue attempting to send messages.
 */
int irc_broadcast_msg(struct irc_msg *msg)
{
	int ret = 0;
	struct irc_server *server;

	list_save_current(irc->servers);
	list_clear_current(irc->servers);
	while (server = list_next(irc->servers))
		ret = irc_send_msg(server, msg);
	list_restore_current(irc->servers);
	return(ret);
}

/**
 * Send the given string to the given channel or nick name on the given
 * server and return -1 if the send fail and 0 on success.
 */
int irc_private_msg(struct irc_server *server, char *name, char *text)
{
	struct irc_msg *msg;
	struct irc_channel *channel;

	if (!(msg = irc_create_msg(IRC_MSG_PRIVMSG, NULL, NULL, 2, name, text)))
		return(-1);
	if (!(list_find(irc->servers, server, 0)))
		return(-2);
	if (irc_send_msg(server, msg))
		return(-3);
//	execute_callback(irc->dispatch, msg);
	irc_dispatch_msg(server, msg);
	irc_destroy_msg(msg);
	return(0);
}

/**
 * Send the given string to the given channel or nick name on the given
 * server as a notice message and return -1 if the send fail and 0 on
 * success.
 */
int irc_notice(struct irc_server *server, char *name, char *text)
{
	struct irc_msg *msg;

	if (!(msg = irc_create_msg(IRC_MSG_NOTICE, NULL, NULL, 2, name, text)))
		return(-1);
	if (!(list_find(irc->servers, server, 0)))
		return(-2);
	if (irc_send_msg(server, msg))
		return(-3);
//	execute_callback(irc->dispatch, msg);
	irc_dispatch_msg(server, msg);
	irc_destroy_msg(msg);
	return(0);
}


/**
 * Return the current channel based on the current window selected.  If no
 * channel is associated with the current window or there is no current window
 * then NULL is returned.
 */
struct irc_channel *irc_current_channel(void)
{
	void *window;

	if (!(window = fe_current_widget()) && !(window = fe_first_widget()))
		return(NULL);
	if (!irc->last_widget || (irc->last_widget != window)) {
		irc->current = irc_find_channel_window(window);
		irc->last_widget = window;
	}
	return(irc->current);
}

/**
 * Find the channel that has the given name and return a pointer to it
 * or return NULL if not found.
 */
struct irc_channel *irc_find_channel(char *name)
{
	struct irc_server *server;
	struct irc_channel *channel;

	list_clear_current(irc->servers);
	while (server = list_next(irc->servers)) {
		if (channel = list_find(server->channels, name, 0))
			return(channel);
	}
	return(NULL);
}

/**
 * Search all the channels in all of the servers and compare the window of
 * those channels with the given window.  Return the pointer to the channel
 * if they match or return NULL if no match is found.
 */
struct irc_channel *irc_find_channel_window(void *window)
{
	struct irc_server *server;
	struct irc_channel *channel;

	list_clear_current(irc->servers);
	while (server = list_next(irc->servers)) {
		if (server->status->window == window)
			return(server->status);
		if (channel = (struct irc_channel *) list_traverse(server->channels, (traverse_t) irc_match_channel_window, window))
			return(channel);
	}
	return(NULL);
}


// TODO should variable dispatch be here since it must access the msgs?
// TODO should we even have/need variable dispatch?

