/*
 * Module Name:		user.c
 * Description:		User Interface Manager
 */

#include <string.h>

#include <stutter/memory.h>
#include <stutter/linear.h>
#include <stutter/string.h>
#include <stutter/macros.h>
#include <stutter/globals.h>
#include <stutter/modules/irc/user.h>

struct irc_user_node {
	struct irc_user user;
	linear_node_v(irc_user_node) ul;
};

struct irc_user_list {
	linear_list_v(irc_user_node) ul;
};

/**
 * Create a new user list and return a pointer to it or return NULL.
 */
struct irc_user_list *irc_create_user_list(void)
{
	struct irc_user_list *list;

	if (!(list = (struct irc_user_list *) memory_alloc(sizeof(struct irc_user_list))))
		return(NULL);
	linear_init_v(list->ul);
	return(list);
}

/**
 * Free the resources used by the given user list.
 */
void irc_destroy_user_list(struct irc_user_list *list)
{
	struct irc_user_node *cur, *tmp;

	linear_foreach_safe_v(list->ul, ul, cur, tmp) {
		if (cur->user.nick)
			destroy_string(cur->user.nick);
		memory_free(cur);
	}
	memory_free(list);
}

/**
 * Add a new user entry with the give nick and bitflags to the given
 * users list.  A pointer to the new user struct is returned or NULL
 * is returned on failure.
 */
struct irc_user *irc_add_user(struct irc_user_list *list, char *nick, int bitflags)
{
	struct irc_user_node *node;

	if (!list || !(node = (struct irc_user_node *) memory_alloc(sizeof(struct irc_user_node))))
		return(NULL);
	node->user.nick = create_string("%s", nick);
	node->user.bitflags = bitflags;
	linear_add_node_sorted_v(list->ul, ul, node, (strcmp_icase(cur->user.nick, nick) > 0));
	return(&node->user);
}

/**
 * Remove the user entry with the given nick from the given list
 * and return 0 or else return 1 if the user was not found.
 */
int irc_remove_user(struct irc_user_list *list, char *nick)
{
	struct irc_user_node *node;

	if (!list)
		return(-1);
	linear_find_node_v(list->ul, ul, node, !strcmp_icase(cur->user.nick, nick));
	if (!node)
		return(1);
	linear_remove_node_v(list->ul, ul, node);
	destroy_string(node->user.nick);
	memory_free(node);
	return(0);
}

/**
 * Find the given user name in the given user list and return a pointer
 * to it or return NULL if it is not found.
 */
struct irc_user *irc_find_user(struct irc_user_list *list, char *nick)
{
	struct irc_user_node *node;

	if (!list)
		return(NULL);
	linear_find_node_v(list->ul, ul, node, !strcmp_icase(cur->user.nick, nick));
	if (!node)
		return(NULL);
	return(&node->user);
}

/**
 * Change the nick for the user entry in the given list and return 0 or
 * return 1 if the user was not found.
 */
int irc_change_user_nick(struct irc_user_list *list, char *oldnick, char *newnick)
{
	struct irc_user_node *node;

	if (!list)
		return(-1);
	linear_find_node_v(list->ul, ul, node, !strcmp_icase(cur->user.nick, oldnick));
	if (!node)
		return(1);
	destroy_string(node->user.nick);
	node->user.nick = create_string("%s", newnick);
	return(0);
}

/**
 * Traverse the given user list and for each user call the given traverse
 * function passing the user struct as the first parameter and the given
 * ptr as the second parameter.  Return 0 if the traverse completes successfully
 * or if the traverse function returns non-zero, the traverse is stopped and the
 * returned value is returned.
 */
int irc_traverse_user_list(struct irc_user_list *list, traverse_t func, void *ptr)
{
	int ret;
	struct irc_user_node *cur;

	linear_foreach_v(list->ul, ul, cur) {
		if ((ret = func(&cur->user, ptr)))
			return(ret);
	}
	return(0);
}


/**
 * Returns the first user in the given user list.
 */
struct irc_user *irc_user_get_first(struct irc_user_list *list)
{
	struct irc_user_node *node;

	if ((node = linear_first_v(list->ul)))
		return(&node->user);
	return(NULL);
}

/**
 * Returns the next user structure in a user list given its previous
 * user structure.
 */
struct irc_user *irc_user_get_next(struct irc_user *user)
{
	struct irc_user_node *node;

	if ((node = linear_next_v(ul, (struct irc_user_node *) user)))
		return(&node->user);
	return(NULL);
}


