/*
 * Module Name:		current.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Current Server\Channel Manager
 */

#include <frontend.h>
#include <modules/irc.h>

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

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(NULL);
	if (!last_window || (last_window != window)) {
		last_channel = irc_server_find_window(window);
		last_window = window;
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

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(NULL);
	if (!last_window || (last_window != window)) {
		last_channel = irc_server_find_window(window);
		last_window = window;
	}
	return(last_channel);
}

