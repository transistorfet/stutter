/*
 * Command Name:	complete.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Nick Complete (Tab Complete) Command
 */

#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/modules/irc.h>

static int size;
static int irc_cmd_complete_traverse(struct irc_user *, char *);

int irc_cmd_complete(char *env, char *args)
{
	void *input;
	char buffer[STRING_SIZE];
	struct irc_channel *channel;

	if (!(channel = irc_current_channel()) || !(input = fe_current_widget("input", NULL)))
		return(-1);
	fe_read(input, buffer, STRING_SIZE);
	if (((size = strlen(buffer)) > 2) && (buffer[size - 2] == ':'))
		size -= 2;
	irc_traverse_user_list(channel->users, (traverse_t) irc_cmd_complete_traverse, buffer);
	fe_print(input, buffer);
	return(0);
}

static int irc_cmd_complete_traverse(struct irc_user *user, char *buffer)
{
	if ((size == -1) || !strncmp_icase(user->nick, buffer, size)) {
		if ((size != -1) && (buffer[size] == ':'))
			size = -1;
		else {
			sprintf(buffer, "%s: ", user->nick);
			return(1);
		}
	}
	return(0);
}

