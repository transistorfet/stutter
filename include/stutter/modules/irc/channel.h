/*
 * Header Name:		channel.h
 * Version:		0.1
 * Description:		Channel Interface Manager Header
 */

#ifndef _STUTTER_MODULES_IRC_CHANNEL_H
#define _STUTTER_MODULES_IRC_CHANNEL_H

#include <stutter/lib/string.h>
#include <stutter/modules/irc/user.h>

#define IRC_CBF_AUTO_REJOIN		0x01

struct irc_server;

struct irc_channel {
	int bitflags;
	char *name;
	string_t topic;
	
	void *window;
	struct irc_user_list *users;
	struct irc_server *server;
};

struct irc_channel_list;

struct irc_channel_list *irc_create_channel_list(void);
void irc_destroy_channel_list(struct irc_channel_list *);
struct irc_channel *irc_add_channel(struct irc_channel_list *, char *, void *, struct irc_server *);
int irc_remove_channel(struct irc_channel_list *, char *);
struct irc_channel *irc_find_channel(struct irc_channel_list *, char *);
struct irc_channel *irc_channel_find_window(struct irc_channel_list *, void *);
int irc_traverse_channel_list(struct irc_channel_list *, traverse_t, void *);

#endif


