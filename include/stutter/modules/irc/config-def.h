/*
 * Header Name:	config-def.h
 * Description:	Default IRC Module Configuration Header
 */

#ifndef _MODULES_IRC_CONFIG_DEF_H
#define _MODULES_IRC_CONFIG_DEF_H

#include <stutter/config-macros.h>
#include <stutter/modules/irc/config-macros.h>

#define DEFAULT_IRC_DEFAULT_PORT		6667
#define DEFAULT_IRC_DEFAULT_NICK		"CaptainPants"

#define DEFAULT_IRC_LOG_SIZE			100
#define DEFAULT_IRC_MAX_NICK			32
#define DEFAULT_IRC_MAX_STRING			512
#define DEFAULT_IRC_PING_WATCHDOG_TIMEOUT	300
#define DEFAULT_IRC_RECONNECT_RETRIES		0

#define DEFAULT_IRC_CHANNEL_BITFLAGS		0x01

#define DEFAULT_IRC_TIMESTAMP			"%d-%b:%H:%M"
#define DEFAULT_IRC_BANNER			"***"

#define DEFAULT_IRC_FMT_CTCP_VERSION		"\022status\022%B [\022timestamp\022%@\022status\022] CTCP Version received from \022nick\022%N\022status\022 (%H)"
#define DEFAULT_IRC_FMT_CTCP_VERSION_REPLY	"\022status\022%B [\022timestamp\022%@\022status\022] CTCP Version reply from \022nick\022%N\022status\022 (%H) %A0"
#define DEFAULT_IRC_FMT_CTCP_UNKNOWN		"\022status\022%B [\022timestamp\022%@\022status\022] Unknown CTCP %T0 received from \022nick\022%N\022status\022 (%H)"
#define DEFAULT_IRC_FMT_CURRENT			"%N!%H %C %M"
#define DEFAULT_IRC_FMT_DEFAULT			"%N!%H %C %M"
#define DEFAULT_IRC_FMT_PRIVATE_ACTION		"[\022timestamp\022%@\022default\022] <*> \022nick\022%N\022default\022 %A0"
#define DEFAULT_IRC_FMT_PRIVATE_ACTION_SELF	"\022bracket\022[\022timestamp\022%@\022bracket\022]\022default\022 <*> \022nick\022%n\022message\022 %A0"
#define DEFAULT_IRC_FMT_PUBLIC_ACTION		"[\022timestamp\022%@\022default\022] * \022nick\022%N\022message\022 %A0"
#define DEFAULT_IRC_FMT_PUBLIC_ACTION_SELF	"\022bracket\022[\022nick\022%@\022bracket\022]\022default\022 * \022nick\022%n\022message\022 %A0"
#define DEFAULT_IRC_FMT_JOIN			"\022status\022%B [\022timestamp\022%@\022status\022] Joined \022channel\022%P1\022status\022: \022nick\022%N\022status\022 (%H)"
#define DEFAULT_IRC_FMT_KICK			"\022status\022%B [\022timestamp\022%@\022status\022] \022nick\022%P2\022status\022 was kicked from \022channel\022%P1\022status\022 by \022nick\022%N\022status\022 (%M)"
#define DEFAULT_IRC_FMT_KICK_SELF		"\022status\022%B [\022timestamp\022%@\022status\022] You were kicked from \022channel\022%P1\022status\022 by \022nick\022%N\022status\022 (%M)"
#define DEFAULT_IRC_FMT_MODE			"\022status\022%B Mode on \022channel\022%P1\022status\022 by \022nick\022%N\022status\022: %P2 %P3 %P4 %P5 %P6 %P7 %P8"
#define DEFAULT_IRC_FMT_NAMES			"\022status\022%B Users on \022channel\022%P3\022status\022: %M"
#define DEFAULT_IRC_FMT_NICK			"\022status\022%B [\022timestamp\022%@\022status\022] nick: \022nick\022%N\022status\022 -> \022nick\022%P1"
#define DEFAULT_IRC_FMT_NOTICE			"%B [\022timestamp\022%@\022default\022] -\022nick\022%N\022default\022- \022message\022%M"
#define DEFAULT_IRC_FMT_NOTICE_SELF		"%B \022bracket\022[\022timestamp\022%@\022bracket\022]\022default\022 -\022nick\022%n\022default\022- \022message\022%M"
#define DEFAULT_IRC_FMT_PART			"\022status\022%B [\022timestamp\022%@\022status\022] Leaving \022channel\022%P1\022status\022: \022nick\022%N\022status\022 (%M)"
#define DEFAULT_IRC_FMT_PING			"\022status\022%B [\022timestamp\022%@\022status\022] Ping received from \022nick\022%N\022status\022 (%H)"
#define DEFAULT_IRC_FMT_PING_REPLY		"\022status\022%B [\022timestamp\022%@\022status\022] Ping reply from \022nick\022%N\022status\022 (%A0)"
#define DEFAULT_IRC_FMT_PRIVATE_MSG		"-[\022timestamp\022%@ \022nick\022%N\022default\022]- \022message\022%M"
#define DEFAULT_IRC_FMT_PRIVATE_MSG_SELF	"-\022bracket\022[\022timestamp\022%@ \022nick\022%n\022default\022 -> \022nick\022%P1\022bracket\022]\022default\022- \022message\022%M"
#define DEFAULT_IRC_FMT_PUBLIC_MSG		"[\022timestamp\022%@ \022nick\022%N\022default\022] \022message\022%M"
#define DEFAULT_IRC_FMT_PUBLIC_MSG_SELF		"\022bracket\022[\022timestamp\022%@ \022nick\022%n\022bracket\022]\022message\022 %M"
#define DEFAULT_IRC_FMT_QUIT			"\022status\022%B [\022timestamp\022%@\022status\022] Quit: \022nick\022%N\022status\022 (%M)"
#define DEFAULT_IRC_FMT_STATUS			"\022status\022%B [\022timestamp\022%@\022status\022] %M"
#define DEFAULT_IRC_FMT_TOPIC			"\022topic\022%B Topic Set by \022nick\022%N\022topic\022 To: %M"
#define DEFAULT_IRC_FMT_WHOISUSER		"\022status\022%B [Whois \022nick\022%P2\022status\022] Realname : %M"
#define DEFAULT_IRC_FMT_WHOISSERVER		"\022status\022%B [Whois \022nick\022%P2\022status\022] Server : \022server\022%P3\022status\022 (%M)"
#define DEFAULT_IRC_FMT_WHOISOPERATOR		"\022status\022%B \022nick\022%P2\022status\022 %M"
#define DEFAULT_IRC_FMT_WHOISIDLE		"\022status\022%B [Whois \022nick\022%P2\022status\022] %P3 %M"
#define DEFAULT_IRC_FMT_WHOISCHANNELS		"\022status\022%B [Whois \022nick\022%P2\022status\022] Channels : \022channel\022%M"
#define DEFAULT_IRC_FMT_WHOISSPECIAL		"\022status\022%B %M"
#define DEFAULT_IRC_FMT_WHOWASUSER		"\022status\022%B [Whowas \022nick\022%P2\022status\022 (%P3@%P4)] Realname : %M"
#define DEFAULT_IRC_FMT_NICKNAMEINUSE		"\022error\022%B [\022timestamp\022%@\022error\022] \022nick\022%P2: \022error\022%M"
#define DEFAULT_IRC_FMT_NOSUCHNICK		"\022error\022%B [\022timestamp\022%@\022error\022] \022nick\022%P2: \022error\022%M"

#define DEFAULT_IRC_QUIT_MSG			"The Pooper Scooper Of Life!"
#define DEFAULT_IRC_VERSION_RESPONSE		"St-St-Stutter " STUTTER_VERSION

#define DEFAULT_IRC_ERR_SERVER_DISCONNECTED	"\022error\022Error: Disconnected from %s"
#define DEFAULT_IRC_ERR_RECONNECT_ERROR		"\022error\022Error: Unable to reconnect to %s"
#define DEFAULT_IRC_ERR_MSG_NOT_ENOUGH_PARAMS	"\022error\022Error: IRC message, %s, doesn't have enough parameters"
#define DEFAULT_IRC_ERR_MSG_TOO_MANY_PARAMS	"\022error\022Error: IRC message, %s, has too many parameters"
#define DEFAULT_IRC_ERR_JOIN_ERROR		"\022error\022Error: Unable to create channel resources after joining %s"
#define DEFAULT_IRC_ERR_QUERY_ERROR		"\022error\022Error: Unable to create query resources for %s"

#define DEFAULT_IRC_BINDINGS()			\
	BIND_KEY("\x09", "irc.complete", "")

#define DEFAULT_IRC_HANDLERS()							\
	ADD_HANDLER("irc.dispatch_msg", NULL, 10, irc_sig_dispatch_msg, NULL)	\
	ADD_HANDLER("irc.dispatch_ctcp", NULL, 5, irc_msg_ctcp_action, NULL)	\
	ADD_HANDLER("irc.dispatch_ctcp", NULL, 5, irc_msg_ctcp_ping, NULL)	\
	ADD_HANDLER("irc.dispatch_ctcp", NULL, 5, irc_msg_ctcp_version, NULL)	\
	ADD_HANDLER("irc.dispatch_ctcp", NULL, 0, irc_msg_ctcp_unknown, NULL)	\
	ADD_HANDLER("base.exec_output", NULL, 10, irc_sig_exec, NULL)		\
	ADD_HANDLER("fe.quit", NULL, 10, irc_sig_quit, NULL)

#define DEFAULT_IRC_COMMANDS()				\
	ADD_COMMAND("complete", irc_cmd_complete)	\
	ADD_COMMAND("ctcp", irc_cmd_ctcp)		\
	ADD_COMMAND("disconnect", irc_cmd_disconnect)	\
	ADD_COMMAND("endquery", irc_cmd_endquery)	\
	ADD_COMMAND("join", irc_cmd_join)		\
	ADD_COMMAND("kick", irc_cmd_kick)		\
	ADD_COMMAND("leave", irc_cmd_leave)		\
	ADD_COMMAND("me", irc_cmd_me)			\
	ADD_COMMAND("mode", irc_cmd_mode)		\
	ADD_COMMAND("msg", irc_cmd_msg)			\
	ADD_COMMAND("names", irc_cmd_names)		\
	ADD_COMMAND("nick", irc_cmd_nick)		\
	ADD_COMMAND("notice", irc_cmd_notice)		\
	ADD_COMMAND("part", irc_cmd_leave)		\
	ADD_COMMAND("ping", irc_cmd_ping)		\
	ADD_COMMAND("query", irc_cmd_query)		\
	ADD_COMMAND("reconnect", irc_cmd_reconnect)	\
	ADD_COMMAND("say", irc_cmd_say)			\
	ADD_COMMAND("server", irc_cmd_server)		\
	ADD_COMMAND("topic", irc_cmd_topic)		\
	ADD_COMMAND("whois", irc_cmd_whois)		\
	ADD_COMMAND("whowas", irc_cmd_whowas)		\
	ADD_COMMAND("", irc_cmd_say)

/*** Joinpoints ***/

#define DEFAULT_IRC_INIT_JOINPOINT(table)	\
	DO_NOTHING();

#define DEFAULT_IRC_RELEASE_JOINPOINT(table)	\
	DO_NOTHING();

#define DEFAULT_IRC_ERROR_JOINPOINT(...)	\
	ERROR_JOINPOINT("irc.error", __VA_ARGS__)

#define DEFAULT_IRC_OUPUT_JOINPOINT(...)	\
	OUTPUT_JOINPOINT("irc.output", __VA_ARGS__)

#define DEFAULT_IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_PRINT_TO_CHANNEL(channel, msg, fmt)

#define DEFAULT_IRC_SERVER_OUTPUT_JOINPOINT(server, msg, fmt)	\
	IRC_PRINT_TO_STATUS(server, msg, fmt)

#define DEFAULT_IRC_MSG_CURRENT_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_DEFAULT_OUTPUT_JOINPOINT(msg, fmt)	\
	IRC_SERVER_OUTPUT_JOINPOINT(msg->server, msg, fmt)

#define DEFAULT_IRC_MSG_INUSE_OUTPUT_JOINPOINT(msg, fmt)	\
	IRC_PRINT_TO_CURRENT(msg, fmt)

#define DEFAULT_IRC_MSG_JOIN_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_KICK_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_MODE_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_NAMES_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_NICK_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_NOTICE_OUTPUT_JOINPOINT(msg, fmt)	\
	IRC_PRINT_TO_CURRENT_AND_STATUS(msg, fmt)

#define DEFAULT_IRC_MSG_PART_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_PRIVMSG_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_QUIT_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_TOPIC_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_CTCP_ACTION_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)

#define DEFAULT_IRC_MSG_CTCP_PING_OUTPUT_JOINPOINT(msg, fmt)	\
	IRC_PRINT_TO_CURRENT_AND_STATUS(msg, fmt)

#define DEFAULT_IRC_MSG_CTCP_VERSION_OUTPUT_JOINPOINT(msg, fmt)	\
	IRC_PRINT_TO_CURRENT_AND_STATUS(msg, fmt)

#define DEFAULT_IRC_MSG_CTCP_UNKNOWN_OUTPUT_JOINPOINT(msg, fmt)	\
	IRC_PRINT_TO_CURRENT(msg, fmt)


/*** START OF DEFAULT ASSIGNMENTS ***/
#ifndef IRC_DEFAULT_PORT
#define IRC_DEFAULT_PORT \
	DEFAULT_IRC_DEFAULT_PORT
#endif
#ifndef IRC_DEFAULT_NICK
#define IRC_DEFAULT_NICK \
	DEFAULT_IRC_DEFAULT_NICK
#endif
#ifndef IRC_LOG_SIZE
#define IRC_LOG_SIZE \
	DEFAULT_IRC_LOG_SIZE
#endif
#ifndef IRC_MAX_NICK
#define IRC_MAX_NICK \
	DEFAULT_IRC_MAX_NICK
#endif
#ifndef IRC_MAX_STRING
#define IRC_MAX_STRING \
	DEFAULT_IRC_MAX_STRING
#endif
#ifndef IRC_PING_WATCHDOG_TIMEOUT
#define IRC_PING_WATCHDOG_TIMEOUT \
	DEFAULT_IRC_PING_WATCHDOG_TIMEOUT
#endif
#ifndef IRC_RECONNECT_RETRIES
#define IRC_RECONNECT_RETRIES \
	DEFAULT_IRC_RECONNECT_RETRIES
#endif
#ifndef IRC_CHANNEL_BITFLAGS
#define IRC_CHANNEL_BITFLAGS \
	DEFAULT_IRC_CHANNEL_BITFLAGS
#endif
#ifndef IRC_TIMESTAMP
#define IRC_TIMESTAMP \
	DEFAULT_IRC_TIMESTAMP
#endif
#ifndef IRC_BANNER
#define IRC_BANNER \
	DEFAULT_IRC_BANNER
#endif
#ifndef IRC_FMT_CTCP_VERSION
#define IRC_FMT_CTCP_VERSION \
	DEFAULT_IRC_FMT_CTCP_VERSION
#endif
#ifndef IRC_FMT_CTCP_VERSION_REPLY
#define IRC_FMT_CTCP_VERSION_REPLY \
	DEFAULT_IRC_FMT_CTCP_VERSION_REPLY
#endif
#ifndef IRC_FMT_CTCP_UNKNOWN
#define IRC_FMT_CTCP_UNKNOWN \
	DEFAULT_IRC_FMT_CTCP_UNKNOWN
#endif
#ifndef IRC_FMT_CURRENT
#define IRC_FMT_CURRENT \
	DEFAULT_IRC_FMT_CURRENT
#endif
#ifndef IRC_FMT_DEFAULT
#define IRC_FMT_DEFAULT \
	DEFAULT_IRC_FMT_DEFAULT
#endif
#ifndef IRC_FMT_PRIVATE_ACTION
#define IRC_FMT_PRIVATE_ACTION \
	DEFAULT_IRC_FMT_PRIVATE_ACTION
#endif
#ifndef IRC_FMT_PRIVATE_ACTION_SELF
#define IRC_FMT_PRIVATE_ACTION_SELF \
	DEFAULT_IRC_FMT_PRIVATE_ACTION_SELF
#endif
#ifndef IRC_FMT_PUBLIC_ACTION
#define IRC_FMT_PUBLIC_ACTION \
	DEFAULT_IRC_FMT_PUBLIC_ACTION
#endif
#ifndef IRC_FMT_PUBLIC_ACTION_SELF
#define IRC_FMT_PUBLIC_ACTION_SELF \
	DEFAULT_IRC_FMT_PUBLIC_ACTION_SELF
#endif
#ifndef IRC_FMT_JOIN
#define IRC_FMT_JOIN \
	DEFAULT_IRC_FMT_JOIN
#endif
#ifndef IRC_FMT_KICK
#define IRC_FMT_KICK \
	DEFAULT_IRC_FMT_KICK
#endif
#ifndef IRC_FMT_KICK_SELF
#define IRC_FMT_KICK_SELF \
	DEFAULT_IRC_FMT_KICK_SELF
#endif
#ifndef IRC_FMT_MODE
#define IRC_FMT_MODE \
	DEFAULT_IRC_FMT_MODE
#endif
#ifndef IRC_FMT_NAMES
#define IRC_FMT_NAMES \
	DEFAULT_IRC_FMT_NAMES
#endif
#ifndef IRC_FMT_NICK
#define IRC_FMT_NICK \
	DEFAULT_IRC_FMT_NICK
#endif
#ifndef IRC_FMT_NOTICE
#define IRC_FMT_NOTICE \
	DEFAULT_IRC_FMT_NOTICE
#endif
#ifndef IRC_FMT_NOTICE_SELF
#define IRC_FMT_NOTICE_SELF \
	DEFAULT_IRC_FMT_NOTICE_SELF
#endif
#ifndef IRC_FMT_PART
#define IRC_FMT_PART \
	DEFAULT_IRC_FMT_PART
#endif
#ifndef IRC_FMT_PING
#define IRC_FMT_PING \
	DEFAULT_IRC_FMT_PING
#endif
#ifndef IRC_FMT_PING_REPLY
#define IRC_FMT_PING_REPLY \
	DEFAULT_IRC_FMT_PING_REPLY
#endif
#ifndef IRC_FMT_PRIVATE_MSG
#define IRC_FMT_PRIVATE_MSG \
	DEFAULT_IRC_FMT_PRIVATE_MSG
#endif
#ifndef IRC_FMT_PRIVATE_MSG_SELF
#define IRC_FMT_PRIVATE_MSG_SELF \
	DEFAULT_IRC_FMT_PRIVATE_MSG_SELF
#endif
#ifndef IRC_FMT_PUBLIC_MSG
#define IRC_FMT_PUBLIC_MSG \
	DEFAULT_IRC_FMT_PUBLIC_MSG
#endif
#ifndef IRC_FMT_PUBLIC_MSG_SELF
#define IRC_FMT_PUBLIC_MSG_SELF \
	DEFAULT_IRC_FMT_PUBLIC_MSG_SELF
#endif
#ifndef IRC_FMT_QUIT
#define IRC_FMT_QUIT \
	DEFAULT_IRC_FMT_QUIT
#endif
#ifndef IRC_FMT_STATUS
#define IRC_FMT_STATUS \
	DEFAULT_IRC_FMT_STATUS
#endif
#ifndef IRC_FMT_TOPIC
#define IRC_FMT_TOPIC \
	DEFAULT_IRC_FMT_TOPIC
#endif
#ifndef IRC_FMT_WHOISUSER
#define IRC_FMT_WHOISUSER \
	DEFAULT_IRC_FMT_WHOISUSER
#endif
#ifndef IRC_FMT_WHOISSERVER
#define IRC_FMT_WHOISSERVER \
	DEFAULT_IRC_FMT_WHOISSERVER
#endif
#ifndef IRC_FMT_WHOISOPERATOR
#define IRC_FMT_WHOISOPERATOR \
	DEFAULT_IRC_FMT_WHOISOPERATOR
#endif
#ifndef IRC_FMT_WHOISIDLE
#define IRC_FMT_WHOISIDLE \
	DEFAULT_IRC_FMT_WHOISIDLE
#endif
#ifndef IRC_FMT_WHOISCHANNELS
#define IRC_FMT_WHOISCHANNELS \
	DEFAULT_IRC_FMT_WHOISCHANNELS
#endif
#ifndef IRC_FMT_WHOISSPECIAL
#define IRC_FMT_WHOISSPECIAL \
	DEFAULT_IRC_FMT_WHOISSPECIAL
#endif
#ifndef IRC_FMT_WHOWASUSER
#define IRC_FMT_WHOWASUSER \
	DEFAULT_IRC_FMT_WHOWASUSER
#endif
#ifndef IRC_FMT_NICKNAMEINUSE
#define IRC_FMT_NICKNAMEINUSE \
	DEFAULT_IRC_FMT_NICKNAMEINUSE
#endif
#ifndef IRC_FMT_NOSUCHNICK
#define IRC_FMT_NOSUCHNICK \
	DEFAULT_IRC_FMT_NOSUCHNICK
#endif
#ifndef IRC_QUIT_MSG
#define IRC_QUIT_MSG \
	DEFAULT_IRC_QUIT_MSG
#endif
#ifndef IRC_VERSION_RESPONSE
#define IRC_VERSION_RESPONSE \
	DEFAULT_IRC_VERSION_RESPONSE
#endif
#ifndef IRC_ERR_SERVER_DISCONNECTED
#define IRC_ERR_SERVER_DISCONNECTED \
	DEFAULT_IRC_ERR_SERVER_DISCONNECTED
#endif
#ifndef IRC_ERR_RECONNECT_ERROR
#define IRC_ERR_RECONNECT_ERROR \
	DEFAULT_IRC_ERR_RECONNECT_ERROR
#endif
#ifndef IRC_ERR_MSG_NOT_ENOUGH_PARAMS
#define IRC_ERR_MSG_NOT_ENOUGH_PARAMS \
	DEFAULT_IRC_ERR_MSG_NOT_ENOUGH_PARAMS
#endif
#ifndef IRC_ERR_MSG_TOO_MANY_PARAMS
#define IRC_ERR_MSG_TOO_MANY_PARAMS \
	DEFAULT_IRC_ERR_MSG_TOO_MANY_PARAMS
#endif
#ifndef IRC_ERR_JOIN_ERROR
#define IRC_ERR_JOIN_ERROR \
	DEFAULT_IRC_ERR_JOIN_ERROR
#endif
#ifndef IRC_ERR_QUERY_ERROR
#define IRC_ERR_QUERY_ERROR \
	DEFAULT_IRC_ERR_QUERY_ERROR
#endif
#ifndef IRC_BINDINGS
#define IRC_BINDINGS() \
	DEFAULT_IRC_BINDINGS()
#endif
#ifndef IRC_HANDLERS
#define IRC_HANDLERS() \
	DEFAULT_IRC_HANDLERS()
#endif
#ifndef IRC_COMMANDS
#define IRC_COMMANDS() \
	DEFAULT_IRC_COMMANDS()
#endif
#ifndef IRC_INIT_JOINPOINT
#define IRC_INIT_JOINPOINT(table) \
	DEFAULT_IRC_INIT_JOINPOINT(table)
#endif
#ifndef IRC_RELEASE_JOINPOINT
#define IRC_RELEASE_JOINPOINT(table) \
	DEFAULT_IRC_RELEASE_JOINPOINT(table)
#endif
#ifndef IRC_ERROR_JOINPOINT
#define IRC_ERROR_JOINPOINT(...) \
	DEFAULT_IRC_ERROR_JOINPOINT(__VA_ARGS__)
#endif
#ifndef IRC_OUPUT_JOINPOINT
#define IRC_OUPUT_JOINPOINT(...) \
	DEFAULT_IRC_OUPUT_JOINPOINT(__VA_ARGS__)
#endif
#ifndef IRC_CHANNEL_OUTPUT_JOINPOINT
#define IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_SERVER_OUTPUT_JOINPOINT
#define IRC_SERVER_OUTPUT_JOINPOINT(server, msg, fmt) \
	DEFAULT_IRC_SERVER_OUTPUT_JOINPOINT(server, msg, fmt)
#endif
#ifndef IRC_MSG_CURRENT_OUTPUT_JOINPOINT
#define IRC_MSG_CURRENT_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_CURRENT_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_DEFAULT_OUTPUT_JOINPOINT
#define IRC_MSG_DEFAULT_OUTPUT_JOINPOINT(msg, fmt) \
	DEFAULT_IRC_MSG_DEFAULT_OUTPUT_JOINPOINT(msg, fmt)
#endif
#ifndef IRC_MSG_INUSE_OUTPUT_JOINPOINT
#define IRC_MSG_INUSE_OUTPUT_JOINPOINT(msg, fmt) \
	DEFAULT_IRC_MSG_INUSE_OUTPUT_JOINPOINT(msg, fmt)
#endif
#ifndef IRC_MSG_JOIN_OUTPUT_JOINPOINT
#define IRC_MSG_JOIN_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_JOIN_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_KICK_OUTPUT_JOINPOINT
#define IRC_MSG_KICK_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_KICK_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_MODE_OUTPUT_JOINPOINT
#define IRC_MSG_MODE_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_MODE_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_NAMES_OUTPUT_JOINPOINT
#define IRC_MSG_NAMES_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_NAMES_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_NICK_OUTPUT_JOINPOINT
#define IRC_MSG_NICK_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_NICK_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_NOTICE_OUTPUT_JOINPOINT
#define IRC_MSG_NOTICE_OUTPUT_JOINPOINT(msg, fmt) \
	DEFAULT_IRC_MSG_NOTICE_OUTPUT_JOINPOINT(msg, fmt)
#endif
#ifndef IRC_MSG_PART_OUTPUT_JOINPOINT
#define IRC_MSG_PART_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_PART_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_PRIVMSG_OUTPUT_JOINPOINT
#define IRC_MSG_PRIVMSG_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_PRIVMSG_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_QUIT_OUTPUT_JOINPOINT
#define IRC_MSG_QUIT_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_QUIT_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_TOPIC_OUTPUT_JOINPOINT
#define IRC_MSG_TOPIC_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_TOPIC_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_CTCP_ACTION_OUTPUT_JOINPOINT
#define IRC_MSG_CTCP_ACTION_OUTPUT_JOINPOINT(channel, msg, fmt) \
	DEFAULT_IRC_MSG_CTCP_ACTION_OUTPUT_JOINPOINT(channel, msg, fmt)
#endif
#ifndef IRC_MSG_CTCP_PING_OUTPUT_JOINPOINT
#define IRC_MSG_CTCP_PING_OUTPUT_JOINPOINT(msg, fmt) \
	DEFAULT_IRC_MSG_CTCP_PING_OUTPUT_JOINPOINT(msg, fmt)
#endif
#ifndef IRC_MSG_CTCP_VERSION_OUTPUT_JOINPOINT
#define IRC_MSG_CTCP_VERSION_OUTPUT_JOINPOINT(msg, fmt) \
	DEFAULT_IRC_MSG_CTCP_VERSION_OUTPUT_JOINPOINT(msg, fmt)
#endif
#ifndef IRC_MSG_CTCP_UNKNOWN_OUTPUT_JOINPOINT
#define IRC_MSG_CTCP_UNKNOWN_OUTPUT_JOINPOINT(msg, fmt) \
	DEFAULT_IRC_MSG_CTCP_UNKNOWN_OUTPUT_JOINPOINT(msg, fmt)
#endif
/*** END OF DEFAULT ASSIGNMENTS ***/

#endif


