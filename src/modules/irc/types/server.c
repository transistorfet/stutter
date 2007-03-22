/*
 * Type Name:		server.c
 * Type Requirements:	channel:irc ; channel-list:irc ; string-ref
 * Description:		IRC Server Type
 */

#include <stdarg.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/memory.h>
#include <stutter/modules/irc.h>

static struct type_s *channel_type;
static struct type_s *string_ref_type;
static struct type_s *channel_list_type;

static void *irc_server_type_create(struct irc_server *, char *, va_list);
static void *irc_server_type_index(struct irc_server *, char *, struct type_s **);
static int irc_server_type_stringify(struct irc_server *, char *, int);

struct type_s *irc_load_server_type(void)
{
	if (!(channel_type = find_type("channel:irc")))
		return(NULL);
	if (!(string_ref_type = find_type("string-ref")))
		return(NULL);
	if (!(channel_list_type = find_type("channel-list:irc")))
		return(NULL);

	return(add_type(
		"server:irc",
		0,
		(type_create_t) irc_server_type_create,
		NULL,
		NULL,
		NULL,
		(type_index_t) irc_server_type_index,
		NULL,
		(type_stringify_t) irc_server_type_stringify,
		NULL
	));
}

/*** Local Functions ***/

static void *irc_server_type_create(struct irc_server *value, char *params, va_list va)
{
	struct irc_server *server;

	if (strcmp(params, "pointer"))
		return(NULL);
	server = va_arg(va, struct irc_server *);
	return(server);
}

static void *irc_server_type_index(struct irc_server *value, char *name, struct type_s **type_ptr)
{
	if (!strcmp(name, "address")) {
		if (type_ptr)
			*type_ptr = string_ref_type;
		return(&value->address);
	}
	else if (!strcmp(name, "nick")) {
		if (type_ptr)
			*type_ptr = string_ref_type;
		return(&value->nick);
	}
	else if (!strncmp(name, "status", 6) && (name[6] == NAME_SEPARATOR)) {
		if (!channel_type->index)
			return(NULL);
		return(channel_type->index(value->status, &name[7], type_ptr));
	}
	else if (!strncmp(name, "channels", 8) && (name[8] == NAME_SEPARATOR)) {
		if (!channel_list_type->index)
			return(NULL);
		return(channel_list_type->index(value->channels, &name[9], type_ptr));
	}
	else
		return(NULL);
}

static int irc_server_type_stringify(struct irc_server *value, char *buffer, int max)
{
	if (!value->address) {
		buffer[0] = '\0';
		return(0);
	}

	strncpy(buffer, value->address, max - 1);
	buffer[max - 1] = '\0';
	return(strlen(buffer));
}


