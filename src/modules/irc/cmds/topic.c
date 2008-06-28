/*
 * Command Name:	topic.c
 * Description:		Topic Command
 */

#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/modules/irc/irc.h>

int irc_cmd_topic(char *env, char *args)
{
	char *name;
	int pos = 0;
	struct irc_msg *msg;
	struct irc_channel *channel;

	if (!(channel = irc_current_channel()))
		return(-1);
	name = util_get_arg(args, &pos);
	if (*name == '\0')
		return(-1);
	if (args[pos] == '\0')
		msg = irc_create_msg(IRC_MSG_TOPIC, NULL, NULL, 1, 0, name);
	else
		msg = irc_create_msg(IRC_MSG_TOPIC, NULL, NULL, 2, 0, name, (!strcmp_icase(&args[pos], "none")) ? "" : &args[pos]);
	if (!msg)
		return(-1);
	irc_send_msg(channel->server, msg);
	return(0);
}


