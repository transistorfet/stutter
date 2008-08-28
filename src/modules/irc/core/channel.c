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
#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/modules/irc/user.h>
#include <stutter/modules/irc/server.h>
#include <stutter/modules/irc/channel.h>

struct variable_type_s irc_channel_type = { {
	OBJECT_TYPE_S(&variable_type),
	"irc_channel",
	sizeof(struct irc_channel),
	NULL,
	(object_init_t) NULL,
	(object_release_t) irc_channel_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) NULL,
	(variable_evaluate_t) NULL
};

void irc_destroy_channel_list(struct irc_channel_list *list)
{
	struct irc_channel *cur, *tmp;

	for (cur = list->head; cur; cur = tmp) {
		tmp = cur->next;
		destroy_object(OBJECT_S(cur));
	}
}

void irc_channel_release(struct irc_channel *channel)
{
	if (channel->name)
		destroy_string(channel->name);
	if (channel->topic)
		destroy_string(channel->topic);
	irc_destroy_user_list(&channel->users);
}

struct irc_channel *irc_add_channel(struct irc_channel_list *list, const char *name, struct irc_server *server)
{
	struct irc_channel *channel;

	if (!(channel = (struct irc_channel *) create_object(OBJECT_TYPE_S(&irc_channel_type), "")))
		return(NULL);
	if (!(channel->name = create_string("%s", name))) {
		destroy_object(OBJECT_S(channel));
		return(NULL);
	}

	#ifdef IRC_CHANNEL_BITFLAGS
	channel->bitflags = IRC_CHANNEL_BITFLAGS;
	#endif
	channel->server = server;

	channel->next = list->head;
	list->head = channel;
	return(channel);
}

int irc_remove_channel(struct irc_channel_list *list, const char *name)
{
	struct irc_channel *cur, *prev;

	for (prev = NULL, cur = list->head; cur; prev = cur, cur = cur->next) {
		if (!strcmp_icase(cur->name, name)) {
			if (prev)
				prev->next = cur->next;
			else
				list->head = cur->next;
			destroy_object(OBJECT_S(cur));
			return(0);
		}
	}
	return(-1);
}

struct irc_channel *irc_find_channel(struct irc_channel_list *list, const char *name)
{
	struct irc_channel *cur;

	for (cur = list->head; cur; cur = cur->next) {
		if (!strcmp_icase(cur->name, name))
			return(cur);
	}
	return(NULL);
}


