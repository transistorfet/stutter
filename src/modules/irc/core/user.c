/*
 * Module Name:		user.c
 * Version:		0.1
 * Module Requirements:	memory ; linear ; string
 * Description:		User Interface Manager
 */

#include <string.h>

#include <stutter/lib/memory.h>
#include <stutter/lib/linear.h>
#include <stutter/lib/string.h>
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
	linear_destroy_list_v(list->ul, ul,
		if (cur->user.nick)
			destroy_string(cur->user.nick);
		memory_free(cur);
	);
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
	node->user.nick = create_string(nick);
	node->user.bitflags = bitflags;
	linear_add_node_v(list->ul, ul, node);

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
	linear_find_node_v(list->ul, ul, node, !strcmp(cur->user.nick, nick));
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
	linear_find_node_v(list->ul, ul, node, !strcmp(cur->user.nick, nick));
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
	linear_find_node_v(list->ul, ul, node, !strcmp(cur->user.nick, oldnick));
	if (!node)
		return(1);
	destroy_string(node->user.nick);
	node->user.nick = create_string(newnick);
	return(0);
}


