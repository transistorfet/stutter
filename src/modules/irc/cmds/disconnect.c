/*
 * Command Name:	disconnect.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Server Disconnect Command
 */

#include <frontend.h>
#include <modules/irc.h>

static int irc_cmd_disconnect_destroy_window(struct irc_channel *, void *);

int irc_cmd_disconnect(char *env, char *args)
{
	struct irc_msg *msg;
	struct irc_server *server;

	if (!(server = irc_current_server()))
		return(-1);
	if (!(msg = irc_create_msg(IRC_MSG_QUIT, NULL, NULL, 1, (*args != '\0') ? args : IRC_QUIT_MSG)))
		return(-1);
	irc_send_msg(server, msg);
	irc_destroy_msg(msg);

	irc_traverse_channel_list(server->channels, (traverse_t) irc_cmd_disconnect_destroy_window, msg);
	irc_server_disconnect(server);
	return(0);
}

/**
 * Destroy the window for each channel belonging to the server that we
 * are disconnecting from and return 0.
 */
static int irc_cmd_disconnect_destroy_window(struct irc_channel *channel, void *ptr)
{
	if (channel->window)
		fe_destroy_widget(channel->window);
	channel->window = NULL;
	return(0);
}


