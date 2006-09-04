/*
 * Header Name:	config.h
 * Description:	Configuration Header (Default)
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#define STUTTER_VERSION			"pre-0.3"

#define FE_NAMESPACE			"fe"
#define BASE_NAMESPACE			"base"
#define IRC_NAMESPACE			"irc"
#define VARIABLE_PATH			"irc;base"

/* Frontend Values */
#define FE_STATUS			"$&fe.status"
#define FE_STATUS_BAR			" $time $irc.current_nick on $irc.current_channel"
#define FE_STATUS_BAR_HEIGHT		1
#define FE_WINDOW_LOG_SIZE		1000
#define FE_WINDOW_WRAP_STRING		"        "

/* Base Module Values */
#define BASE_TIME			"%H:%M"
#define BASE_DATE			"%d-%b"
#define BASE_TIMESTAMP			"%d-%b:%H:%M"
#define BASE_DEFAULT_SET_TYPE		"string"
#define BASE_PARSE_PATH_VARIABLE	"path"

/* IRC Module Values */
#define IRC_DEFAULT_PORT		6667
#define IRC_DEFAULT_NICK		"CaptainPants"

#define IRC_LOG_SIZE			100
#define IRC_MAX_NICK			20
#define IRC_MAX_STRING			512

#define IRC_CHANNEL_BITFLAGS		0x01

#define IRC_TIMESTAMP			"%d-%b:%H:%M"
#define IRC_BANNER			"***"

#define IRC_FMT_CTCP_VERSION		"%B [%@] CTCP Version received from %N (%H)"
#define IRC_FMT_CTCP_VERSION_REPLY	"%B [%@] CTCP Version reply from %N (%H) %M"
#define IRC_FMT_CURRENT			"%N!%H %C %M"
#define IRC_FMT_DEFAULT			"%N!%H %C %M"
#define IRC_FMT_PRIVATE_ACTION		"[%@] <*> %N %M"
#define IRC_FMT_PRIVATE_ACTION_SELF	"${fe.bold}[${fe.bold}%@${fe.bold}]${fe.bold} <*> %n %M"
#define IRC_FMT_PUBLIC_ACTION		"[%@] * %N %M"
#define IRC_FMT_PUBLIC_ACTION_SELF	"${fe.bold}[${fe.bold}%@${fe.bold}]${fe.bold} * %n %M"
#define IRC_FMT_JOIN			"%B [%@] Joined %P1: %N (%H)"
#define IRC_FMT_KICK			"%B [%@] %P2 was kicked from %P1 by %N (%M)"
#define IRC_FMT_KICK_SELF		"%B [%@] You were kicked from %P1 by %N (%M)"
//#define IRC_FMT_NAMES			"$banner Users on <channel>%P3</channel>: <msg>%T</msg>"
//#define IRC_FMT_NAMES			"${fe.colour.status}$banner Users on %P3: %M"
#define IRC_FMT_NAMES			"%B Users on %P3: %M"
#define IRC_FMT_NICK			"%B [%@] nick: %N -> %P1"
#define IRC_FMT_NOTICE			"%B [%@] -%N- %M"
#define IRC_FMT_NOTICE_SELF		"%B ${fe.bold}[${fe.bold}%@${fe.bold}]${fe.bold} -%n- %M"
#define IRC_FMT_PART			"%B [%@] Leaving %P1: %N (%M)"
#define IRC_FMT_PING			"%B [%@] Ping received from %N (%H)"
#define IRC_FMT_PING_REPLY		"%B [%@] Ping reply from %N (%M)"
#define IRC_FMT_PRIVATE_MSG		"-[%@ %N]- %M"
#define IRC_FMT_PRIVATE_MSG_SELF	"-${fe.bold}[${fe.bold}%@ %n -> %N${fe.bold}]${fe.bold}- %M"
#define IRC_FMT_PUBLIC_MSG		"[%@ %N] %M"
#define IRC_FMT_PUBLIC_MSG_SELF		"${fe.bold}[${fe.bold}%@ %n${fe.bold}]${fe.bold} %M"
#define IRC_FMT_QUIT			"%B [%@] Quit: %N (%M)"
#define IRC_FMT_TOPIC			"%B Topic Set by %N To: %M"
#define IRC_FMT_WHOISUSER		"%B [Whois %P2] Realname : %M"
#define IRC_FMT_WHOISSERVER		"%B [Whois %P2] Server : %P3 (%M)"
#define IRC_FMT_WHOISOPERATOR		"%B %P2 %M"
#define IRC_FMT_WHOISIDLE		"%B [Whois %P2] %P3 %M"
#define IRC_FMT_WHOISCHANNELS		"%B [Whois %P2] Channels : %M"
#define IRC_FMT_WHOISSPECIAL		"%B %M"

#define IRC_QUIT_MSG			"The Pooper Scooper Of Life!"
#define IRC_VERSION_RESPONSE		"St-St-Stutter " STUTTER_VERSION

#define ERR_MSG_UNKNOWN_COMMAND		"*** Unknown Command"

/* General Values */
#define COMMAND_PREFIX			"/"
#define DEFAULT_COMMAND			""
#define NAME_SEPARATOR			'.'

#define STRING_SIZE			512

#define DOT_FILE			"~/.stutterrc"

#define ERR_MSG_UNABLE_TO_OPEN_FILE	"Error: Unable to open the file, %s"
#define ERR_MSG_SERVER_DISCONNECTED	"Error: Disconnected from %s"
#define ERR_MSG_RECONNECT_ERROR		"Error: Unable to reconnect to %s"
#define ERR_MSG_JOIN_ERROR		"Error: Unable to create channel resources after joining %s"

#define LOAD_MODULES()			\
	init_base();			\
	init_irc();

#define RELEASE_MODULES()		\
	release_irc();			\
	release_base();

#define BIND_KEYS()				\
	BIND_KEY("\n", "base.parse", "")	\
	BIND_KEY("\x18", "base.next", "")	\
	BIND_KEY("\x15", "base.clearline", "")	\
	BIND_KEY("\x11", "base.previous", "")	\
	BIND_KEY("\x10", "base.scroll", "1")	\
	BIND_KEY("\x19", "base.scroll", "-1")	\
	BIND_KEY("\x09", "irc.complete", "")


/*** Base Module ***/

#define BASE_HANDLERS()							\
	ADD_HANDLER("error_general", NULL, 10, base_sig_print, NULL)	\
	ADD_HANDLER("base.error", NULL, 10, base_sig_print, NULL)

#define BASE_COMMANDS()					\
	ADD_COMMAND("bind", base_cmd_bind)		\
	ADD_COMMAND("clear", base_cmd_clear)		\
	ADD_COMMAND("clearline", base_cmd_clearline)	\
	ADD_COMMAND("echo", base_cmd_echo)		\
	ADD_COMMAND("get", base_cmd_get)		\
	ADD_COMMAND("next", base_cmd_next)		\
	ADD_COMMAND("parse", base_cmd_parse)		\
	ADD_COMMAND("previous", base_cmd_previous)	\
	ADD_COMMAND("quit", base_cmd_quit)		\
	ADD_COMMAND("remove", base_cmd_remove)		\
	ADD_COMMAND("scroll", base_cmd_scroll)		\
	ADD_COMMAND("set", base_cmd_set)		\
	ADD_COMMAND("source", base_cmd_source)

/*** IRC Module ***/

#define IRC_HANDLERS()								\
	ADD_HANDLER("irc_dispatch_msg", NULL, 10, irc_sig_dispatch_msg, NULL)	\
	ADD_HANDLER("irc_dispatch_ctcp", NULL, 10, irc_msg_ctcp_action, NULL)	\
	ADD_HANDLER("irc_dispatch_ctcp", NULL, 10, irc_msg_ctcp_ping, NULL)	\
	ADD_HANDLER("irc_dispatch_ctcp", NULL, 10, irc_msg_ctcp_version, NULL)	\
	ADD_HANDLER("quit", NULL, 10, irc_sig_quit, NULL)

#define IRC_COMMANDS()					\
	ADD_COMMAND("complete", irc_cmd_complete)	\
	ADD_COMMAND("ctcp", irc_cmd_ctcp)		\
	ADD_COMMAND("disconnect", irc_cmd_disconnect)	\
	ADD_COMMAND("join", irc_cmd_join)		\
	ADD_COMMAND("leave", irc_cmd_leave)		\
	ADD_COMMAND("me", irc_cmd_me)			\
	ADD_COMMAND("msg", irc_cmd_msg)			\
	ADD_COMMAND("names", irc_cmd_names)		\
	ADD_COMMAND("nick", irc_cmd_nick)		\
	ADD_COMMAND("notice", irc_cmd_notice)		\
	ADD_COMMAND("part", irc_cmd_leave)		\
	ADD_COMMAND("ping", irc_cmd_ping)		\
	ADD_COMMAND("reconnect", irc_cmd_reconnect)	\
	ADD_COMMAND("say", irc_cmd_say)			\
	ADD_COMMAND("server", irc_cmd_server)		\
	ADD_COMMAND("topic", irc_cmd_topic)		\
	ADD_COMMAND("whois", irc_cmd_whois)		\
	ADD_COMMAND("", irc_cmd_say)


#define STUTTER_INIT(argc, argv) {				\
	struct variable_s *var;					\
	if (var = find_variable(NULL, "base.source"))		\
		var->type->evaluate(var->value, DOT_FILE);	\
}

#endif


