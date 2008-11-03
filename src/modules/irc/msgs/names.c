/*
 * Module Name:		names.c
 * Description:		Name Notification Message
 */

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/signal.h>
#include <stutter/modules/irc/irc.h>

/**
 * Update the user information and print message to channel.
 */
int irc_msg_names(char *env, struct irc_msg *msg)
{
	char *name;
	int pos = 0;
	int bitflags;
	struct irc_channel *channel;
	char buffer[LARGE_STRING_SIZE];

	if (!(channel = irc_find_channel(&msg->server->channels, msg->params[2])) && !(channel = irc_current_channel()))
		return(-1);
	if (irc_format_msg(msg, IRC_FMT_NAMES, buffer, LARGE_STRING_SIZE) >= 0)
		signal_emit(channel->signal, buffer);

	strncpy(buffer, msg->text, LARGE_STRING_SIZE - 1);
	buffer[LARGE_STRING_SIZE] = '\0';
	while (buffer[pos] != '\0') {
		bitflags = 0;
		name = util_get_arg(&buffer[pos], &pos);
		if ((name[0] == '@') || (name[0] == '+')) {
			bitflags = (name[0] == '@') ? IRC_UBF_MODE_OP : IRC_UBF_MODE_VOICE ;
			name = &name[1];
		}
		if (!irc_find_user(&channel->users, name))
			irc_add_user(&channel->users, name, bitflags);
	}
	return(0);
}


