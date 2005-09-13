/*
 * Module Name:		names.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Name Notification Message
 */

#include FRONTEND_H
#include <macros.h>
#include <modules/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_names(char *env, struct irc_msg *msg)
{
	int bitflags;
	char *str, *name;
	struct irc_server *server;
	struct irc_channel *channel;

	if (!(server = irc_current_server()))
		return(-1);
	if (!(channel = irc_get_channel(server, msg->params[2])))
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
		if (!irc_get_user(channel, name))
			irc_add_user(channel, name, bitflags);
	}
	return(0);
}


