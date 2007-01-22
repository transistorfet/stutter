/*
 * Module Name:		names.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Name Notification Message
 */

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_names(char *env, struct irc_msg *msg)
{
	int bitflags;
	char *str, *name;
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(msg->server->channels, msg->params[2])) && !(channel = irc_current_channel()))
		return(-1);
	IRC_MSG_NAMES_OUTPUT_JOINPOINT(channel, msg, IRC_FMT_NAMES)

	str = msg->text;
	while (*str != '\0') {
		bitflags = 0;
		get_param_m(str, name, ' ');
		if ((name[0] == '@') || (name[0] == '+')) {
			bitflags = (name[0] == '@') ? IRC_UBF_MODE_OP : IRC_UBF_MODE_VOICE ;
			name = &name[1];
		}
		if (!irc_find_user(channel->users, name))
			irc_add_user(channel->users, name, bitflags);
	}
	return(0);
}


