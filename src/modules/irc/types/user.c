/*
 * Type Name:		user.c
 * Type Requirements:	string-ref
 * Description:		IRC User Type
 */

#include <stdarg.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/modules/irc/irc.h>

static struct type_s *string_ref_type;

static void *irc_user_type_create(struct irc_user *, char *, va_list);
static void *irc_user_type_index(struct irc_user *, char *, struct type_s **);
static int irc_user_type_stringify(struct irc_user *, char *, int);

struct type_s *irc_load_user_type(void)
{
	if (!(string_ref_type = find_type("string-ref")))
		return(NULL);

	return(add_type(
		"user:irc",
		0,
		(type_create_t) irc_user_type_create,
		NULL,
		NULL,
		NULL,
		(type_index_t) irc_user_type_index,
		NULL,
		(type_stringify_t) irc_user_type_stringify,
		NULL
	));
}

/*** Local Functions ***/

static void *irc_user_type_create(struct irc_user *value, char *params, va_list va)
{
	struct irc_user *user;

	if (strcmp(params, "pointer"))
		return(NULL);
	user = va_arg(va, struct irc_user *);
	return(user);
}

static void *irc_user_type_index(struct irc_user *value, char *name, struct type_s **type_ptr)
{
	if (!strcmp(name, "nick")) {
		if (type_ptr)
			*type_ptr = string_ref_type;
		return(&value->nick);
	}
	else
		return(NULL);
}

static int irc_user_type_stringify(struct irc_user *value, char *buffer, int max)
{
	if (!value->nick) {
		buffer[0] = '\0';
		return(0);
	}

	strncpy(buffer, value->nick, max - 1);
	buffer[max - 1] = '\0';
	return(strlen(buffer));
}


