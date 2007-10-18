/*
 * Module Name:		mode.c
 * Description:		Mode Change Notification Message
 */

#include CONFIG_H
#include <stutter/string.h>
#include <stutter/frontend.h>
#include <stutter/modules/irc.h>

static inline int irc_msg_mode_convert(struct irc_channel *, struct irc_msg *);

int irc_msg_mode(char *env, struct irc_msg *msg)
{
	struct irc_channel *channel;

	if ((channel = irc_find_channel(msg->server->channels, msg->params[0])))
		irc_msg_mode_convert(channel, msg);
	else if (!(channel = msg->server->status))
		return(-1);
	IRC_MSG_MODE_OUTPUT_JOINPOINT(channel, msg, IRC_FMT_MODE)
	return(0);
}

/*** Local Functions ***/

static inline int irc_msg_mode_convert(struct irc_channel *channel, struct irc_msg *msg)
{
	int i, j, cur, method;
	struct irc_user *user;

	for (i = 1;i < msg->num_params;i++) {
		cur = i;
		if ((msg->params[i][0] != '+') && (msg->params[i][0] != '-'))
			continue;
		method = (msg->params[i][0] == '+') ? 1 : 0;
		for (j = 1;msg->params[i][j] != '\0';j++) {
			switch (msg->params[i][j]) {
				case 'o': {
					if ((user = irc_find_user(channel->users, msg->params[++cur]))) {
						if (method)
							user->bitflags |= IRC_UBF_MODE_OP;
						else
							user->bitflags &= ~IRC_UBF_MODE_OP;
					}
					break;
				}
				case 'v': {
					if ((user = irc_find_user(channel->users, msg->params[++cur]))) {
						if (method)
							user->bitflags |= IRC_UBF_MODE_VOICE;
						else
							user->bitflags &= ~IRC_UBF_MODE_VOICE;
					}
					break;
				}
				case 'm': {
					if (method)
						channel->bitflags |= IRC_CBF_MODE_MODERATED;
					else
						channel->bitflags &= ~IRC_CBF_MODE_MODERATED;
					break;
				}
				case 't': {
					if (method)
						channel->bitflags |= IRC_CBF_MODE_OPS_TOPIC;
					else
						channel->bitflags &= ~IRC_CBF_MODE_OPS_TOPIC;
					break;
				}
				case 'n': {
					if (method)
						channel->bitflags |= IRC_CBF_MODE_LOCAL_SEND_ONLY;
					else
						channel->bitflags &= ~IRC_CBF_MODE_LOCAL_SEND_ONLY;
					break;
				}
				case 'i': {
					if (method)
						channel->bitflags |= IRC_CBF_MODE_INVITE_ONLY;
					else
						channel->bitflags &= ~IRC_CBF_MODE_INVITE_ONLY;
					break;
				}
				case 'p': {
					if (method)
						channel->bitflags |= IRC_CBF_MODE_PRIVATE;
					else
						channel->bitflags &= ~IRC_CBF_MODE_PRIVATE;
					break;
				}
				case 's': {
					if (method)
						channel->bitflags |= IRC_CBF_MODE_SECRET;
					else
						channel->bitflags &= ~IRC_CBF_MODE_SECRET;
					break;
				}
				case 'k':
				case 'l':
				case 'b': {
					cur++;
					break;
				}
				default:
					break;
			}
		}
		i = cur;
	}
	return(0);
}


