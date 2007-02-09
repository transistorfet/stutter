/*
 * Module Name:		channel.c
 * Version:		0.1
 * Module Requirements:	memory ; linear ; string ; user
 * Description:		Channel Interface Manager
 */

#include <stdlib.h>
#include <string.h>

#include <stutter/memory.h>
#include <stutter/linear.h>
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/modules/irc/user.h>
#include <stutter/modules/irc/channel.h>

struct irc_channel_node {
	struct irc_channel channel;
	linear_node_v(irc_channel_node) cl;
};

struct irc_channel_list {
	linear_list_v(irc_channel_node) cl;
};

/**
 * Create a new list of channels.
 */
struct irc_channel_list *irc_create_channel_list(void)
{
	struct irc_channel_list *list;

	if (!(list = (struct irc_channel_list *) memory_alloc(sizeof(struct irc_channel_list))))
		return(NULL);
	linear_init_v(list->cl);
	return(list);
}

/**
 * Free the resources used by the given channel list.
 */
void irc_destroy_channel_list(struct irc_channel_list *list)
{
	struct irc_channel_node *cur, *tmp;

	linear_foreach_safe_v(list->cl, cl, cur, tmp) {
		if (cur->channel.topic)
			destroy_string(cur->channel.topic);
		if (cur->channel.users)
			irc_destroy_user_list(cur->channel.users);
		memory_free(cur);
	}
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

	if (!(node = (struct irc_channel_node *) memory_alloc(sizeof(struct irc_channel_node) + strlen(name) + 1)))
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
	linear_add_node_v(list->cl, cl, node);

	return(&node->channel);
}

/**
 * Remove the channel with the given name from the given list.  If the
 * channel is not found then 1 is returned otherwise 0.
 */
int irc_remove_channel(struct irc_channel_list *list, char *name)
{
	struct irc_channel_node *node;

	linear_find_node_v(list->cl, cl, node, !strcmp(cur->channel.name, name));
	if (!node)
		return(1);
	linear_remove_node_v(list->cl, cl, node);
	if (node->channel.topic)
		destroy_string(node->channel.topic);
	if (node->channel.users)
		irc_destroy_user_list(node->channel.users);
	memory_free(node);
	return(0);
}

/**
 * Find the channel with the given name in the given list and return
 * a pointer to the channel struct or NULL if not found.  
 */
struct irc_channel *irc_find_channel(struct irc_channel_list *list, char *name)
{
	struct irc_channel_node *node;

	linear_find_node_v(list->cl, cl, node, !strcmp(cur->channel.name, name));
	if (!node)
		return(NULL);
	return(&node->channel);
}

/**
 * Find the channel with the given window pointer in the given list
 * and return a pointer to the channel struct or NULL if not found.  
 */
struct irc_channel *irc_channel_find_window(struct irc_channel_list *list, void *window)
{
	struct irc_channel_node *cur;

	linear_foreach_v(list->cl, cl, cur) {
		if (cur->channel.window == window)
			return(&cur->channel);
	}
	return(NULL);
}

/**
 * Traverse the given channel list and for each channel call the given traverse
 * function passing the channel struct as the first parameter and the given
 * ptr as the second parameter.  Return 0 if the traverse completes successfully
 * or if the traverse function returns non-zero, the traverse is stopped and the
 * returned value is returned.
 */
int irc_traverse_channel_list(struct irc_channel_list *list, traverse_t func, void *ptr)
{
	int ret;
	struct irc_channel_node *cur;

	linear_foreach_v(list->cl, cl, cur) {
		if (ret = func(&cur->channel, ptr))
			return(ret);
	}
	return(0);
}


/**
 * Returns the first channel in the given channel list.
 */
struct irc_channel *irc_channel_get_first(struct irc_channel_list *list)
{
	struct irc_channel_node *node;

	if (node = linear_first_v(list->cl))
		return(&node->channel);
	return(NULL);
}

/**
 * Returns the next channel structure in a channel list given its previous
 * channel structure.
 */
struct irc_channel *irc_channel_get_next(struct irc_channel *channel)
{
	struct irc_channel_node *node;

	if (node = linear_next_v(cl, (struct irc_channel_node *) channel))
		return(&node->channel);
	return(NULL);
}



