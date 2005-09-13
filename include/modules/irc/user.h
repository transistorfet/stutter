/*
 * Header Name:		user.h
 * Version:		0.1
 * Description:		User Interface Manager Header
 */

#ifndef _USER_H
#define _USER_H

#include <nit/string.h>

#define IRC_UBF_OP		0x80
#define IRC_UBF_VOICE		0x40

struct irc_user {
	string_t name;
	int bitflags;
};

#define irc_create_user_list()		create_list(0, (compare_t) irc_compare_user, (destroy_t) irc_destroy_user)

struct irc_user *irc_create_user(char *, int);
int irc_destroy_user(struct irc_user *);
int irc_compare_user(struct irc_user *, char *);
int irc_change_user_nick(struct irc_user *, char *);

#endif


