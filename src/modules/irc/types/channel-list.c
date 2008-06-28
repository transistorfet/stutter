/*
 * Type Name:		channel-list.c
 * Type Requirements:	channel:irc
 * Description:		IRC Channel List Type
 */

#include <stdarg.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/modules/irc/irc.h>

static struct type_s *channel_type;

static void *irc_channel_list_create(struct irc_channel_list *, char *, va_list);
static void *irc_channel_list_index(struct irc_channel_list *, char *, struct type_s **);

struct type_s *irc_load_channel_list(void)
{
	if (!(channel_type = find_type("channel:irc")))
		return(NULL);

	return(add_type(
		"channel-list:irc",
		0,
		(type_create_t) irc_channel_list_create,
		NULL,
		NULL,
		NULL,
		(type_index_t) irc_channel_list_index,
		NULL,
		NULL,
		NULL
	));
}

/*** Local Functions ***/

static void *irc_channel_list_create(struct irc_channel_list *value, char *params, va_list va)
{
	if (strcmp(params, "pointer"))
		return(NULL);
	value = va_arg(va, struct irc_channel_list *);
	return(value);
}

static void *irc_channel_list_index(struct irc_channel_list *value, char *name, struct type_s **type_ptr)
{
	int len;
	struct irc_channel *channel;
	char buffer[NAME_STRING_SIZE];

	for (len = 0; (name[len] != '\0') && (name[len] != NAME_SEPARATOR); len++)
		buffer[len] = name[len];
	buffer[len] = '\0';

	if (!(channel = irc_find_channel(value, buffer)))
		return(NULL);
	else if (name[len] != '\0') {
		if (!channel_type->index)
			return(NULL);
		return(channel_type->index(channel, &name[len + 1], type_ptr));
	}
	else {
		if (type_ptr)
			*type_ptr = channel_type;
		return(channel);
	}
}


