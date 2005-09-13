/*
 * Module Name:		channel.c
 * Version:		0.1
 * Module Requirements:	list ; string ; user
 * Description:		Channel Interface Manager
 */

#include <stdlib.h>
#include <string.h>

#include <nit/list.h>
#include <nit/string.h>
#include <modules/irc/user.h>
#include <modules/irc/channel.h>


/**
 * Allocate and initialize resources for a channel with the give name.
 */
struct irc_channel *irc_create_channel(char *name, void *window, struct irc_server *server)
{
	struct irc_channel *channel;

	if (!(channel = (struct irc_channel *) malloc(sizeof(struct irc_channel) + strlen(name) + 1)))
		return(NULL);
	memset(channel, '\0', sizeof(struct irc_channel));
	channel->name = (char *) (((size_t) channel) + sizeof(struct irc_channel));
	strcpy(channel->name, name);

	channel->window = window;
	channel->server = server;

	if (!(channel->users = irc_create_user_list())) {
		irc_destroy_channel(channel);
		return(NULL);
	}

	return(channel);
}

/**
 * Free all resources used by the given channel
 */
int irc_destroy_channel(struct irc_channel *channel)
{
	if (!channel)
		return(1);
	destroy_string(channel->topic);
	if (channel->users)
		destroy_list(channel->users);
	free(channel);
	return(0);
}

/**
 * Compare the given channel and name and if they have the same channel name then
 * return 0 otherwise a non-zero value.
 */
int irc_compare_channel(struct irc_channel *channel, char *name)
{
	return(strcmp(channel->name, name));
}

/**
 * If the window pointer of the given channel matches the given window pointer then
 * return the given channel, otherwise return NULL.
 */
struct irc_channel *irc_match_channel_window(struct irc_channel *channel, void *window)
{
	if (channel->window == window)
		return(channel);
	return(NULL);
}




