/*
 * Type Name:		channel.c
 * Type Requirements:	string-ref
 * Description:		IRC Channel Type
 */

#include <stdarg.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/modules/irc/irc.h>

static struct type_s *user_list_type;
static struct type_s *string_ref_type;

static void *irc_channel_type_create(struct irc_channel *, char *, va_list);
static void *irc_channel_type_index(struct irc_channel *, char *, struct type_s **);
static int irc_channel_type_stringify(struct irc_channel *, char *, int);

struct type_s *irc_load_channel_type(void)
{
	if (!(user_list_type = find_type("user-list:irc")))
		return(NULL);
	if (!(string_ref_type = find_type("string-ref")))
		return(NULL);

	return(add_type(
		"channel:irc",
		0,
		(type_create_t) irc_channel_type_create,
		NULL,
		NULL,
		NULL,
		(type_index_t) irc_channel_type_index,
		NULL,
		(type_stringify_t) irc_channel_type_stringify,
		NULL
	));
}

/*** Local Functions ***/

static void *irc_channel_type_create(struct irc_channel *value, char *params, va_list va)
{
	struct irc_channel *channel;

	if (strcmp(params, "pointer"))
		return(NULL);
	channel = va_arg(va, struct irc_channel *);
	return(channel);
}

static void *irc_channel_type_index(struct irc_channel *value, char *name, struct type_s **type_ptr)
{
	if (!strcmp(name, "name")) {
		if (type_ptr)
			*type_ptr = string_ref_type;
		return(&value->name);
	}
	else if (!strcmp(name, "topic")) {
		if (type_ptr)
			*type_ptr = string_ref_type;
		return(&value->topic);
	}
	else if (!strncmp(name, "users", 5) && (name[5] == NAME_SEPARATOR)) {
		if (!user_list_type->index)
			return(NULL);
		return(user_list_type->index(value->users, &name[6], type_ptr));
	}
	else
		return(NULL);
}

static int irc_channel_type_stringify(struct irc_channel *value, char *buffer, int max)
{
	if (!value->name) {
		buffer[0] = '\0';
		return(0);
	}

	strncpy(buffer, value->name, max - 1);
	buffer[max - 1] = '\0';
	return(strlen(buffer));
}


