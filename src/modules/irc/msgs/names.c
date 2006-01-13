/*
 * Module Name:		names.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Name Notification Message
 */

#include <frontend.h>
#include <lib/macros.h>
#include <modules/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_names(struct irc_server *server, struct irc_msg *msg)
{
	int bitflags;
	char *str, *name;
	struct irc_channel *channel;

	if (!(channel = irc_find_channel(server->channels, msg->params[2])))
		return(-1);

	if (!(str = irc_format_msg(msg, IRC_FMT_NAMES)))
		return(-1);
	fe_print(channel->window, str);

	str = msg->text;
	while (*str != '\0') {
		bitflags = 0;
		get_param_m(str, name, ' ');
		if ((name[0] == '@') || (name[0] == '+')) {
			bitflags = (name[0] == '@') ? IRC_UBF_OP : IRC_UBF_VOICE ;
			name = &name[1];
		}
		if (!irc_find_user(channel->users, name))
			irc_add_user(channel->users, name, bitflags);
	}
	return(0);
}


