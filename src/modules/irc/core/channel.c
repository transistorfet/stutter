/*
 * Module Name:		channel.c
 * Version:		0.1
 * Module Requirements:	memory ; string ; user
 * Description:		Channel Interface Manager
 */

#include <stdlib.h>
#include <string.h>

#include <nit/types.h>
#include <nit/memory.h>
#include <nit/string.h>
#include <nit/linear.h>
#include <modules/irc/user.h>
#include <modules/irc/channel.h>

#define chanlist			channel_mangle
#define channel_mangle(name)		channel_##name
#define channel_list_field(name)	name
#define channel_node_field(name)	name
#define channel_access(list, name)	list->channel_list_field(name)
#define channel_compare(node, key)	strcmp(node->channel.name, key)

struct irc_channel_node {
	struct irc_channel channel;
	linear_node_fields_v(chanlist, struct irc_channel_node);
};

struct irc_channel_list {
	linear_list_fields_v(chanlist, struct irc_channel_node);
};

/**
 * Create a new list of channels.
 */
struct irc_channel_list *irc_create_channel_list(void)
{
	struct irc_channel_list *list;

	if (!(list = (struct irc_channel_list *) memory_alloc(sizeof(struct irc_channel_list))))
		return(NULL);
	linear_init_list_v(chanlist, list);
	return(list);
}

/**
 * Free the resources used by the given channel list.
 */
void irc_destroy_channel_list(struct irc_channel_list *list)
{
	struct irc_channel_node *tmp, *cur;

	linear_destroy_list_v(chanlist, list,
		linear_release_node_v(chanlist, cur);
		if (cur->channel.topic)
			destroy_string(cur->channel.topic);
		if (cur->channel.users)
			irc_destroy_user_list(cur->channel.users);
		memory_free(cur);
	);
	linear_release_list_v(chanlist, list);
	memory_free(list);
}

/**
 * Add a channel with the give name, window handle, and belonging to the
 * given server to the given list.  A pointer to the channel struct is
 * return or NULL on failure.
 */
struct irc_channel *irc_add_channel(struct irc_channel_list *list, char *name, void *window, struct irc_server *server)
{
	struct irc_channel_node *node;

	if (!list || !(node = (struct irc_channel_node *) memory_alloc(sizeof(struct irc_channel_node) + strlen(name) + 1)))
		return(NULL);
	memset(node, '\0', sizeof(struct irc_channel_node));

	if (!(node->channel.users = irc_create_user_list())) {
		memory_free(node);
		return(NULL);
	}

	node->channel.name = (char *) (((size_t) node) + sizeof(struct irc_channel_node));
	strcpy(node->channel.name, name);

	#ifdef IRC_CHANNEL_BITFLAGS
	node->channel.bitflags = IRC_CHANNEL_BITFLAGS;
	#endif
	node->channel.window = window;
	node->channel.server = server;
	linear_add_node_v(chanlist, list, node);

	return(&node->channel);
}

/**
 * Remove the channel with the given name from the given list.  If the
 * channel is not found then 1 is returned otherwise 0.
 */
int irc_remove_channel(struct irc_channel_list *list, char *name)
{
	struct irc_channel_node *cur, *prev;

	if (!list)
		return(-1);
	linear_remove_node_v(chanlist, list, name);
	if (!cur)
		return(1);
	if (cur->channel.topic)
		destroy_string(cur->channel.topic);
	if (cur->channel.users)
		irc_destroy_user_list(cur->channel.users);
	memory_free(cur);
	return(0);
}

/**
 * Find the channel with the given name in the given list and return
 * a pointer to the channel struct or NULL if not found.  
 */
struct irc_channel *irc_find_channel(struct irc_channel_list *list, char *name)
{
	struct irc_channel_node *cur;

	if (!list)
		return(NULL);
	linear_find_node_v(chanlist, list, name);
	if (!cur)
		return(NULL);
	return(&cur->channel);
}

/**
 * Find the channel with the given window pointer in the given list
 * and return a pointer to the channel struct or NULL if not found.  
 */
struct irc_channel *irc_channel_find_window(struct irc_channel_list *list, void *window)
{
	struct irc_channel_node *cur;

	if (!list)
		return(NULL);
	linear_traverse_list_v(chanlist, list,
		if (cur->channel.window == window)
			return(&cur->channel);
	);
	return(NULL);
}

/**
 * Traverse the given channel list and for each channel call the given traverse
 * function passing the channel struct as the first parameter and the given
 * ptr as the second parameter.  Return 0 if the traverse completes successfully.
 */
int irc_traverse_channel_list(struct irc_channel_list *list, traverse_t func, void *ptr)
{
	struct irc_channel_node *cur;

	if (!list)
		return(-1);
	linear_traverse_list_v(chanlist, list,
		func(&cur->channel, ptr);
	);
	return(0);
}



