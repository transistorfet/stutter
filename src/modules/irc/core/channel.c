/*
 * Module Name:		channel.c
 * Description:		Channel Interface Manager
 */

#include <stdlib.h>
#include <string.h>

#include <stutter/memory.h>
#include <stutter/string.h>
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/globals.h>
#include <stutter/modules/irc/user.h>
#include <stutter/modules/irc/server.h>
#include <stutter/modules/irc/channel.h>

static int irc_handle_purge_window(struct irc_channel *, void *);
static inline void irc_destroy_channel_node(struct irc_channel_node *);

struct variable_type_s irc_channel_type = { {
	OBJECT_TYPE_S(&variable_type),
	"irc_channel",
	sizeof(struct irc_channel),
	NULL,
	(object_init_t) irc_channel_init,
	(object_release_t) irc_channel_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) NULL,
	(variable_evaluate_t) NULL
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
		irc_destroy_channel_node(cur);
	}
	memory_free(list);
}



int irc_channel_init(struct irc_channel *channel, const char *params, va_list va)
{
	// TODO this isn't finish
	// TODO should we pass the server pointer and the channel name during init?
	if (!(channel->users = irc_create_user_list()))
		return(-1);

	#ifdef IRC_CHANNEL_BITFLAGS
	channel->bitflags = IRC_CHANNEL_BITFLAGS;
	#endif
	channel->server = server;
	return(0);
}

void irc_channel_release(struct irc_channel *channel)
{


}




/**
 * Add a channel with the give name, window handle, and belonging to the
 * given server to the given list.  A pointer to the channel struct is
 * return or NULL on failure.
 */
struct irc_channel *irc_add_channel(struct irc_channel_list *list, const char *name, void *window, struct irc_server *server)
{
	struct irc_channel *channel;

	if (!(channel = (struct irc_channel *) memory_alloc(sizeof(struct irc_channel) + strlen(name) + 1)))
		return(NULL);
	memset(node, '\0', sizeof(struct irc_channel));

	if (!(channel->users = irc_create_user_list())) {
		memory_free(node);
		return(NULL);
	}

	channel->name = (char *) offset_after_struct_m(node, 0);
	strcpy(channel->name, name);

	#ifdef IRC_CHANNEL_BITFLAGS
	channel->bitflags = IRC_CHANNEL_BITFLAGS;
	#endif
	channel->window = window;
	channel->server = server;
	linear_add_node_v(list->cl, cl, node);
	node->handler = signal_connect(window, "purge_object", 10, (signal_t) irc_handle_purge_window, &node->channel);
	return(channel);
}

/**
 * Remove the channel with the given name from the given list.  If the
 * channel is not found then 1 is returned otherwise 0.
 */
int irc_remove_channel(struct irc_channel_list *list, const char *name)
{
	struct irc_channel_node *node;

	linear_find_node_v(list->cl, cl, node, !strcmp_icase(cur->channel.name, name));
	if (!node)
		return(1);
	linear_remove_node_v(list->cl, cl, node);
	irc_destroy_channel_node(node);
	return(0);
}

/**
 * Find the channel with the given name in the given list and return
 * a pointer to the channel struct or NULL if not found.  
 */
struct irc_channel *irc_find_channel(struct irc_channel_list *list, const char *name)
{
	struct irc_channel_node *node;

	linear_find_node_v(list->cl, cl, node, !strcmp_icase(cur->channel.name, name));
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
		if ((ret = func(&cur->channel, ptr)))
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

	if ((node = linear_first_v(list->cl)))
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

	if ((node = linear_next_v(cl, (struct irc_channel_node *) channel)))
		return(&node->channel);
	return(NULL);
}

/*** Local Functions ***/

/**
 * Frees the resources for the given channel and sends the appropriate messages
 * to the IRC server determined by examining the channel name.
 */
static int irc_handle_purge_window(struct irc_channel *channel, void *window)
{
	if (channel->window != window)
		return(-1);
	channel->window = NULL;
	signal_disconnect(((struct irc_channel_node *) channel)->handler);
	((struct irc_channel_node *) channel)->handler = NULL;

	if (!strcmp_icase(channel->name, IRC_SERVER_STATUS_CHANNEL))
		irc_destroy_server(channel->server);
	else if ((channel->name[0] == '#') || (channel->name[0] == '&') || (channel->name[0] == '+') || (channel->name[0] == '!'))
		irc_leave_channel(channel->server, channel->name);
	else
		irc_remove_channel(channel->server->channels, channel->name);
	return(0);
}

/**
 * Destroy the given channel node freeing all resources.
 */
static inline void irc_destroy_channel_node(struct irc_channel_node *node)
{
	if (node->handler)
		signal_disconnect(node->handler);
	if (node->channel.topic)
		destroy_string(node->channel.topic);
	if (node->channel.window)
		fe_destroy_widget(node->channel.window);
	if (node->channel.users)
		irc_destroy_user_list(node->channel.users);
	memory_free(node);
}


