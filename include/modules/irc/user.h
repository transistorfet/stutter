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
	string_t nick;
	int bitflags;
};

struct irc_user_list;

struct irc_user_list *irc_create_user_list(void);
void irc_destroy_user_list(struct irc_user_list *);
struct irc_user *irc_add_user(struct irc_user_list *, char *, int);
int irc_remove_user(struct irc_user_list *, char *);
struct irc_user *irc_find_user(struct irc_user_list *, char *);
int irc_change_user_nick(struct irc_user_list *, char *, char *);

#endif


