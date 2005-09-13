/*
 * Header Name:		channel.h
 * Version:		0.1
 * Description:		Channel Interface Manager Header
 */

#ifndef _CHANNEL_H
#define _CHANNEL_H

#include <nit/list.h>
#include <nit/string.h>
#include <modules/irc/user.h>

#define IRC_CBF_AUTO_REJOIN		0x01

struct irc_server;

struct irc_channel {
	int bitflags;
	char *name;
	string_t topic;
	
	void *window;
	struct list_s *users;
	struct irc_server *server;
};

#define irc_create_channel_list()		create_list(0, (compare_t) irc_compare_channel, (destroy_t) irc_destroy_channel)

#define irc_add_user(channel, name, bitflags)	list_add(channel->users, irc_create_user(name, bitflags))
#define irc_remove_user(channel, name)		list_delete(channel->users, name)
#define irc_get_user(channel, name)		list_find(channel->users, name, 0)

struct irc_channel *irc_create_channel(char *, void *, struct irc_server *);
int irc_destroy_channel(struct irc_channel *);
int irc_compare_channel(struct irc_channel *, char *);
struct irc_channel *irc_match_channel_window(struct irc_channel *, void *);

#endif


