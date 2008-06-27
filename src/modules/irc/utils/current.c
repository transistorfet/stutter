/*
 * Module Name:		current.c
 * Description:		Current Server\Channel Manager
 */

#include <stutter/string.h>
#include <stutter/modules/irc.h>

static void *last_window = NULL;
static struct irc_channel *last_channel = NULL;

/**
 * Return a pointer to the server that is associated with
 * the current window or NULL if no server is not associated
 * with the current window.
 */
struct irc_server *irc_current_server(void)
{
	void *window;

	if (!(window = fe_get_target(NULL, "text")))
		return(NULL);
	if (!last_window || (last_window != window)) {
		last_window = window;
		last_channel = irc_server_find_window(window);
	}
	if (last_channel)
		return(last_channel->server);
	return(NULL);
}

/**
 * Return a pointer to the channel that is associated with
 * the current window or NULL if no channel is not associated
 * with the current window.
 */
struct irc_channel *irc_current_channel(void)
{
	void *window;

	if (!(window = fe_get_target(NULL, "text")))
		return(NULL);
	if (!last_window || (last_window != window)) {
		last_window = window;
		last_channel = irc_server_find_window(window);
	}
	return(last_channel);
}

/**
 * Copy the nick on the current server to the given buffer.
 */
int irc_stringify_nick(void *ptr, char *buffer, int max)
{
	struct irc_server *server;

	if (!(server = irc_current_server()))
		return(-1);
	return(copy_string(server->nick, buffer, max));
}

/**
 * Copy the current channel name to the given buffer.
 */
int irc_stringify_channel(void *ptr, char *buffer, int max)
{
	struct irc_channel *channel;

	if (!(channel = irc_current_channel()))
		return(-1);
	return(copy_string(channel->name, buffer, max));
}


