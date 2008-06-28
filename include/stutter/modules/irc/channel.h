/*
 * Header Name:		channel.h
 * Description:		Channel Interface Manager Header
 */

#ifndef _STUTTER_MODULES_IRC_CHANNEL_H
#define _STUTTER_MODULES_IRC_CHANNEL_H

#include <stutter/string.h>
#include <stutter/modules/irc/user.h>

#define IRC_CBF_MODE_PRIVATE		0x0001
#define IRC_CBF_MODE_SECRET		0x0002
#define IRC_CBF_MODE_INVITE_ONLY	0x0004
#define IRC_CBF_MODE_OPS_TOPIC		0x0008
#define IRC_CBF_MODE_LOCAL_SEND_ONLY	0x0010
#define IRC_CBF_MODE_MODERATED		0x0020

#define IRC_CBF_AUTO_REJOIN		0x0100

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
struct irc_channel *irc_add_channel(struct irc_channel_list *, const char *, void *, struct irc_server *);
int irc_remove_channel(struct irc_channel_list *, const char *);
struct irc_channel *irc_find_channel(struct irc_channel_list *, const char *);
struct irc_channel *irc_channel_find_window(struct irc_channel_list *, void *);
int irc_traverse_channel_list(struct irc_channel_list *, traverse_t, void *);

struct irc_channel *irc_channel_get_first(struct irc_channel_list *);
struct irc_channel *irc_channel_get_next(struct irc_channel *);

#endif


