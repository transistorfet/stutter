/*
 * Module Name:		user.c
 * Version:		0.1
 * Module Requirements:	string ; memory
 * Description:		User Interface Manager
 */

#include <string.h>

#include <nit/string.h>
#include <nit/memory.h>
#include <modules/irc/user.h>


/**
 * Allocate and initialize resources for a user entry with the give name.
 */
struct irc_user *irc_create_user(char *name, int bitflags)
{
	struct irc_user *user;

	if (!(user = (struct irc_user *) memory_alloc(sizeof(struct irc_user))))
		return(NULL);
	memset(user, '\0', sizeof(struct irc_user));
	user->name = create_string(name);
	user->bitflags = bitflags;

	return(user);
}

/**
 * Free all resources used by the given user entry.
 */
int irc_destroy_user(struct irc_user *user)
{
	if (!user)
		return(1);
	destroy_string(user->name);
	memory_free(user);
	return(0);
}

/**
 * Compare the given user and name and if they have the same name then
 * return 0.  If the left string is less than the right string then -1 will be
 * returned.  Otherwise 1 is returned.
 */
int irc_compare_user(struct irc_user *user, char *name)
{
	return(strcmp(user->name, name));
}

/**
 * Free the existing user name, allocate memory for the new name and
 * copy the new name into the new memory.
 */
int irc_change_user_nick(struct irc_user *user, char *name)
{
	destroy_string(user->name);
	user->name = create_string(name);
	return(0);
}


