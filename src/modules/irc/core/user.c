/*
 * Module Name:		user.c
 * Description:		User Interface Manager
 */

#include <string.h>

#include <stutter/memory.h>
#include <stutter/string.h>
#include <stutter/macros.h>
#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/modules/irc/user.h>

struct variable_type_s irc_user_type = { {
	OBJECT_TYPE_S(&variable_type),
	"irc_user",
	sizeof(struct irc_user),
	NULL,
	(object_init_t) NULL,
	(object_release_t) irc_user_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) NULL,
	(variable_evaluate_t) NULL
};

void irc_destroy_user_list(struct irc_user_list *list)
{
	struct irc_user *cur, *tmp;

	for (cur = list->head; cur; cur = tmp) {
		tmp = cur->next;
		destroy_object(OBJECT_S(cur));
	}
}

void irc_user_release(struct irc_user *user)
{
	if (user->nick)
		destroy_string(user->nick);
}


struct irc_user *irc_add_user(struct irc_user_list *list, const char *nick, int bitflags)
{
	struct irc_user *user;

	if (!(user = (struct irc_user *) create_object(OBJECT_TYPE_S(&irc_user_type), "")))
		return(NULL);
	if (!(user->nick = create_string("%s", nick))) {
		destroy_object(OBJECT_S(user));
		return(NULL);
	}
	user->bitflags = bitflags;

	user->next = list->head;
	list->head = user;
	return(user);
}

int irc_remove_user(struct irc_user_list *list, const char *nick)
{
	struct irc_user *cur, *prev;

	for (prev = NULL, cur = list->head; cur; prev = cur, cur = cur->next) {
		if (!strcmp_icase(cur->nick, nick)) {
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

struct irc_user *irc_find_user(struct irc_user_list *list, const char *nick)
{
	struct irc_user *cur;

	for (cur = list->head; cur; cur = cur->next) {
		if (!strcmp_icase(cur->nick, nick))
			return(cur);
	}
	return(NULL);
}

int irc_change_user_nick(struct irc_user_list *list, const char *oldnick, const char *newnick)
{
	struct irc_user *cur;

	for (cur = list->head; cur; cur = cur->next) {
		if (!strcmp_icase(cur->nick, oldnick)) {
			destroy_string(cur->nick);
			if (!(cur->nick = create_string("%s", newnick)))
				return(-1);
			return(0);
		}
	}
	return(-1);
}



