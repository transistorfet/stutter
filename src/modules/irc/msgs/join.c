/*
 * Module Name:		join.c
 * Description:		Join Channel Notification Message
 */

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/output.h>
#include <stutter/modules/irc/irc.h>

int irc_msg_join(char *env, struct irc_msg *msg)
{
	struct irc_channel *channel;
	char buffer[LARGE_STRING_SIZE];

	if (!strcmp_icase(msg->server->nick, msg->nick)) {
		if ((channel = irc_find_channel(&msg->server->channels, msg->params[0]))
		    || (channel = irc_add_channel(&msg->server->channels, msg->params[0], msg->server)))
			// TODO convert this
			; //fe_show_widget(window);
		else {
			irc_leave_channel(msg->server, msg->params[0]);
			// TODO this should output to a server error/status signal
			OUTPUT_ERROR(IRC_ERR_JOIN_ERROR, msg->params[0]);
			return(-1);
		}
	}
	else if (!(channel = irc_find_channel(&msg->server->channels, msg->params[0])))
		return(-1);

	irc_add_user(&channel->users, msg->nick, 0);
	if (irc_format_msg(msg, IRC_FMT_JOIN, buffer, LARGE_STRING_SIZE) >= 0)
		signal_emit(channel->signal, buffer);
	return(0);
}


