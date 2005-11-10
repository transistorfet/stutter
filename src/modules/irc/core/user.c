/*
 * Module Name:		user.c
 * Version:		0.1
 * Module Requirements:	string ; memory
 * Description:		User Interface Manager
 */

#include <string.h>

#include <nit/string.h>
#include <nit/memory.h>
#include <nit/linear.h>
#include <modules/irc/user.h>

#define userlist			user_mangle
#define user_mangle(name)		user_##name
#define user_list_field(name)		name
#define user_node_field(name)		name
#define user_access(list, name)		list->user_node_field(name)
#define user_compare(node, key)		strcmp(node->user.nick, key)

struct irc_user_node {
	struct irc_user user;
	linear_node_fields_v(userlist, struct irc_user_node);
};

struct irc_user_list {
	linear_list_fields_v(userlist, struct irc_user_node);
};

/**
 * Create a new user list and return a pointer to it or return NULL.
 */
struct irc_user_list *irc_create_user_list(void)
{
	struct irc_user_list *list;

	if (!(list = (struct irc_user_list *) memory_alloc(sizeof(struct irc_user_list))))
		return(NULL);
	linear_init_list_v(userlist, list);
	return(list);
}

/**
 * Free the resources used by the given user list.
 */
void irc_destroy_user_list(struct irc_user_list *list)
{
	struct irc_user_node *tmp, *cur;

	linear_destroy_list_v(userlist, list,
		linear_release_node_v(userlist, cur);
		if (cur->user.nick)
			destroy_string(cur->user.nick);
		memory_free(cur);
	);
	linear_release_list_v(userlist, list);
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
	linear_add_node_v(userlist, list, node);

	return(&node->user);
}

/**
 * Remove the user entry with the given nick from the given list
 * and return 0 or else return 1 if the user was not found.
 */
int irc_remove_user(struct irc_user_list *list, char *nick)
{
	struct irc_user_node *cur, *prev;

	if (!list)
		return(-1);
	linear_remove_node_v(userlist, list, nick);
	if (!cur)
		return(1);
	destroy_string(cur->user.nick);
	memory_free(cur);
	return(0);
}

/**
 * Find the given user name in the given user list and return a pointer
 * to it or return NULL if it is not found.
 */
struct irc_user *irc_find_user(struct irc_user_list *list, char *nick)
{
	struct irc_user_node *cur;

	if (!list)
		return(NULL);
	linear_find_node_v(userlist, list, nick);
	if (!cur)
		return(NULL);
	return(&cur->user);
}

/**
 * Change the nick for the user entry in the given list and return 0 or
 * return 1 if the user was not found.
 */
int irc_change_user_nick(struct irc_user_list *list, char *oldnick, char *newnick)
{
	struct irc_user_node *cur;

	if (!list)
		return(-1);
	linear_find_node_v(userlist, list, oldnick);
	if (!cur)
		return(1);
	destroy_string(cur->user.nick);
	cur->user.nick = create_string(newnick);
	return(0);
}


