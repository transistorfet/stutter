/*
 * Header Name:		user.h
 * Description:		User Interface Manager Header
 */

#ifndef _STUTTER_MODULES_IRC_USER_H
#define _STUTTER_MODULES_IRC_USER_H

#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/variable.h>

#define IRC_UBF_MODE_AWAY		0x0001
#define IRC_UBF_MODE_INVISIBLE		0x0002
#define IRC_UBF_MODE_WALLOPS		0x0004
#define IRC_UBF_MODE_RESTRICTED		0x0008
#define IRC_UBF_MODE_OP			0x0010
#define IRC_UBF_MODE_LOCAL_OP		0x0020
#define IRC_UBF_MODE_VOICE		0x0040
#define IRC_UBF_MODE_SERVER_NOTICES	0x0080

#define IRC_USER(ptr)			( (struct irc_user *) (ptr) )

struct irc_user {
	struct variable_s variable;
	struct irc_user *next;
	string_t nick;
	int bitflags;
};

struct irc_user_list {
	struct irc_user *head;
};

extern struct variable_type_s irc_user_type;

void irc_destroy_user_list(struct irc_user_list *);
void irc_user_release(struct irc_user *);

struct irc_user *irc_add_user(struct irc_user_list *, const char *, int);
int irc_remove_user(struct irc_user_list *, const char *);
struct irc_user *irc_find_user(struct irc_user_list *, const char *);
int irc_change_user_nick(struct irc_user_list *, const char *, const char *);

#endif


