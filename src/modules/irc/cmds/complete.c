/*
 * Command Name:	complete.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		Nick Complete (Tab Complete) Command
 */

#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

struct irc_complete_info_s {
	int find_next : 1;
	int no_colon : 1;
	int return_next : 1;
	int size;
	int max;
	char *str;
};

static int irc_cmd_complete_traverse(struct irc_user *, struct irc_complete_info_s *);

int irc_cmd_complete(char *env, char *args)
{
	int i;
	char *str;
	void *input;
	char buffer[STRING_SIZE];
	struct irc_channel *channel;
	struct irc_complete_info_s info = { 0, 0, 0, 0, 0 };

	if (!(channel = irc_current_channel()) || !(input = fe_current_widget("input", NULL)))
		return(-1);
	fe_read(input, buffer, STRING_SIZE);

	info.str = buffer;
	info.size = strlen(buffer);
	info.max = STRING_SIZE;

	if (info.size > 2) {
		if ((info.str[info.size - 1] == ' ') && (info.str[info.size - 2] != ' ')) {
			info.find_next = 1;
			info.size -= (info.str[info.size - 2] == ':') ? 2 : 1;
		}
		for (i = info.size - 1;i >= 0;i--) {
			if (buffer[i] == ' ') {
				info.str = &buffer[i + 1];
				info.no_colon = 1;
				info.max = STRING_SIZE - i;
				info.size -= (i + 1);
				break;
			}
		}
	}

	irc_traverse_user_list(channel->users, (traverse_t) irc_cmd_complete_traverse, &info);
	fe_print(input, buffer);
	return(0);
}

static int irc_cmd_complete_traverse(struct irc_user *user, struct irc_complete_info_s *info)
{
	if (info->return_next || !strncmp_icase(user->nick, info->str, info->size)) {
		if (info->find_next && !info->return_next)
			info->return_next = 1;
		else if (info->no_colon) {
			snprintf(info->str, info->max, "%s ", user->nick);
			return(1);
		}
		else {
			snprintf(info->str, info->max, "%s: ", user->nick);
			return(1);
		}
	}
	return(0);
}

