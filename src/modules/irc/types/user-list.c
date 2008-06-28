/*
 * Type Name:		user-list.c
 * Type Requirements:	user:irc
 * Description:		IRC User List Type
 */

#include <stdarg.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/modules/irc/irc.h>

static struct type_s *user_type;

static void *irc_user_list_create(struct irc_user_list *, char *, va_list);
static void *irc_user_list_index(struct irc_user_list *, char *, struct type_s **);

struct type_s *irc_load_user_list(void)
{
	if (!(user_type = find_type("user:irc")))
		return(NULL);

	return(add_type(
		"user-list:irc",
		0,
		(type_create_t) irc_user_list_create,
		NULL,
		NULL,
		NULL,
		(type_index_t) irc_user_list_index,
		NULL,
		NULL,
		NULL
	));
}

/*** Local Functions ***/

static void *irc_user_list_create(struct irc_user_list *value, char *params, va_list va)
{
	if (strcmp(params, "pointer"))
		return(NULL);
	value = va_arg(va, struct irc_user_list *);
	return(value);
}

static void *irc_user_list_index(struct irc_user_list *value, char *name, struct type_s **type_ptr)
{
	int len;
	struct irc_user *user;
	char buffer[NAME_STRING_SIZE];

	for (len = 0; (name[len] != '\0') && (name[len] != NAME_SEPARATOR); len++)
		buffer[len] = name[len];
	buffer[len] = '\0';

	if (!(user = irc_find_user(value, buffer)))
		return(NULL);
	else if (name[len] != '\0') {
		if (!user_type->index)
			return(NULL);
		return(user_type->index(user, &name[len + 1], type_ptr));
	}
	else {
		if (type_ptr)
			*type_ptr = user_type;
		return(user);
	}
}


