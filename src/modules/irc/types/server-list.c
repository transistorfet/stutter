/*
 * Type Name:		server-list.c
 * Type Requirements:	server:irc
 * Description:		IRC Server List Type
 */

#include <stdarg.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/modules/irc/irc.h>

static struct type_s *server_type;

static void *irc_server_list_create(void *, char *, va_list);
static void *irc_server_list_index(void *, char *, struct type_s **);

struct type_s *irc_load_server_list(void)
{
	if (!(server_type = find_type("server:irc")))
		return(NULL);

	return(add_type(
		"server-list:irc",
		0,
		(type_create_t) irc_server_list_create,
		NULL,
		NULL,
		NULL,
		(type_index_t) irc_server_list_index,
		NULL,
		NULL,
		NULL
	));
}

/*** Local Functions ***/

static void *irc_server_list_create(void *value, char *params, va_list va)
{
	if (*params != '\0')
		return(NULL);
	return((void *) 1);
}

static void *irc_server_list_index(void *value, char *name, struct type_s **type_ptr)
{
	int len;
	struct irc_server *server;
	char buffer[NAME_STRING_SIZE];

	// TODO do something about the server name (can you have a network name instead of just address?
	for (len = 0;(name[len] != '\0') && (name[len] != NAME_SEPARATOR);len++) {
		if (name[len] == '_')
			buffer[len] = '.';
		else
			buffer[len] = name[len];
	}
	buffer[len] = '\0';

	if (!(server = irc_find_server(buffer)))
		return(NULL);
	else if (name[len] != '\0') {
		if (!server_type->index)
			return(NULL);
		return(server_type->index(server, &name[len + 1], type_ptr));
	}
	else {
		if (type_ptr)
			*type_ptr = server_type;
		return(server);
	}
}


