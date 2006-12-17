/*
 * Header Name:	config-def.h
 * Description:	Default IRC Module Configuration Header
 */

#ifndef _MODULES_IRC_CONFIG_DEF_H
#define _MODULES_IRC_CONFIG_DEF_H

#include <stutter/config-macros.h>

#define DEFAULT_IRC_DEFAULT_PORT		6667
#define DEFAULT_IRC_DEFAULT_NICK		"CaptainPants"

#define DEFAULT_IRC_LOG_SIZE			100
#define DEFAULT_IRC_MAX_NICK			20
#define DEFAULT_IRC_MAX_STRING			512

#define DEFAULT_IRC_CHANNEL_BITFLAGS		0x01

#define DEFAULT_IRC_TIMESTAMP			"%d-%b:%H:%M"
#define DEFAULT_IRC_BANNER			"***"

#define DEFAULT_IRC_FMT_CTCP_VERSION		"%B [%@] CTCP Version received from %N (%H)"
#define DEFAULT_IRC_FMT_CTCP_VERSION_REPLY	"%B [%@] CTCP Version reply from %N (%H) %M"
#define DEFAULT_IRC_FMT_CURRENT			"%N!%H %C %M"
#define DEFAULT_IRC_FMT_DEFAULT			"%N!%H %C %M"
#define DEFAULT_IRC_FMT_PRIVATE_ACTION		"[%@] <*> %N %M"
#define DEFAULT_IRC_FMT_PRIVATE_ACTION_SELF	"${fe.bold}[${fe.bold}%@${fe.bold}]${fe.bold} <*> %n %M"
#define DEFAULT_IRC_FMT_PUBLIC_ACTION		"[%@] * %N %M"
#define DEFAULT_IRC_FMT_PUBLIC_ACTION_SELF	"${fe.bold}[${fe.bold}%@${fe.bold}]${fe.bold} * %n %M"
#define DEFAULT_IRC_FMT_JOIN			"%B [%@] Joined %P1: %N (%H)"
#define DEFAULT_IRC_FMT_KICK			"%B [%@] %P2 was kicked from %P1 by %N (%M)"
#define DEFAULT_IRC_FMT_KICK_SELF		"%B [%@] You were kicked from %P1 by %N (%M)"
#define DEFAULT_IRC_FMT_MODE			"%B Mode on %P1 by %N: %P2 %P3 %P4 %P5 %P6 %P7 %P8"
//#define DEFAULT_IRC_FMT_NAMES			"$banner Users on <channel>%P3</channel>: <msg>%T</msg>"
//#define DEFAULT_IRC_FMT_NAMES			"${fe.colour.status}$banner Users on %P3: %M"
#define DEFAULT_IRC_FMT_NAMES			"%B Users on %P3: %M"
#define DEFAULT_IRC_FMT_NICK			"%B [%@] nick: %N -> %P1"
#define DEFAULT_IRC_FMT_NOTICE			"%B [%@] -%N- %M"
#define DEFAULT_IRC_FMT_NOTICE_SELF		"%B ${fe.bold}[${fe.bold}%@${fe.bold}]${fe.bold} -%n- %M"
#define DEFAULT_IRC_FMT_PART			"%B [%@] Leaving %P1: %N (%M)"
#define DEFAULT_IRC_FMT_PING			"%B [%@] Ping received from %N (%H)"
#define DEFAULT_IRC_FMT_PING_REPLY		"%B [%@] Ping reply from %N (%M)"
#define DEFAULT_IRC_FMT_PRIVATE_MSG		"-[%@ %N]- %M"
#define DEFAULT_IRC_FMT_PRIVATE_MSG_SELF	"-${fe.bold}[${fe.bold}%@ %n -> %P1${fe.bold}]${fe.bold}- %M"
#define DEFAULT_IRC_FMT_PUBLIC_MSG		"[%@ %N] %M"
#define DEFAULT_IRC_FMT_PUBLIC_MSG_SELF		"${fe.bold}[${fe.bold}%@ %n${fe.bold}]${fe.bold} %M"
#define DEFAULT_IRC_FMT_QUIT			"%B [%@] Quit: %N (%M)"
#define DEFAULT_IRC_FMT_TOPIC			"%B Topic Set by %N To: %M"
#define DEFAULT_IRC_FMT_WHOISUSER		"%B [Whois %P2] Realname : %M"
#define DEFAULT_IRC_FMT_WHOISSERVER		"%B [Whois %P2] Server : %P3 (%M)"
#define DEFAULT_IRC_FMT_WHOISOPERATOR		"%B %P2 %M"
#define DEFAULT_IRC_FMT_WHOISIDLE		"%B [Whois %P2] %P3 %M"
#define DEFAULT_IRC_FMT_WHOISCHANNELS		"%B [Whois %P2] Channels : %M"
#define DEFAULT_IRC_FMT_WHOISSPECIAL		"%B %M"
#define DEFAULT_IRC_FMT_WHOWASUSER		"%B [Whowas %P2 (%P3@%P4)] Realname : %M"
#define DEFAULT_IRC_FMT_NOSUCHNICK		"%B %P2 %M"

#define DEFAULT_IRC_QUIT_MSG			"The Pooper Scooper Of Life!"
#define DEFAULT_IRC_VERSION_RESPONSE		"St-St-Stutter " STUTTER_VERSION

#define DEFAULT_IRC_ERR_SERVER_DISCONNECTED	"Error: Disconnected from %s"
#define DEFAULT_IRC_ERR_RECONNECT_ERROR		"Error: Unable to reconnect to %s"
#define DEFAULT_IRC_ERR_JOIN_ERROR		"Error: Unable to create channel resources after joining %s"
#define DEFAULT_IRC_ERR_QUERY_ERROR		"Error: Unable to create query resources for %s"

#define DEFAULT_IRC_BINDINGS()			\
	BIND_KEY("\x09", "irc.complete", "")

#define DEFAULT_IRC_HANDLERS()							\
	ADD_HANDLER("irc.dispatch_msg", NULL, 10, irc_sig_dispatch_msg, NULL)	\
	ADD_HANDLER("irc.dispatch_ctcp", NULL, 10, irc_msg_ctcp_action, NULL)	\
	ADD_HANDLER("irc.dispatch_ctcp", NULL, 10, irc_msg_ctcp_ping, NULL)	\
	ADD_HANDLER("irc.dispatch_ctcp", NULL, 10, irc_msg_ctcp_version, NULL)	\
	ADD_HANDLER("base.exec_output", NULL, 10, irc_sig_exec, NULL)		\
	ADD_HANDLER("fe.quit", NULL, 10, irc_sig_quit, NULL)

#define DEFAULT_IRC_COMMANDS()				\
	ADD_COMMAND("complete", irc_cmd_complete)	\
	ADD_COMMAND("ctcp", irc_cmd_ctcp)		\
	ADD_COMMAND("disconnect", irc_cmd_disconnect)	\
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

#define DEFAULT_IRC_INIT_JOINPOINT(table)	\
	DO_NOTHING();

#define DEFAULT_IRC_RELEASE_JOINPOINT(table)	\
	DO_NOTHING();

#define DEFAULT_IRC_ERROR_JOINPOINT(...)	\
	EMIT_STRING("irc.error", NULL, __VA_ARGS__);


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
/*** END OF DEFAULT ASSIGNMENTS ***/


#endif

