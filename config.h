/*
 * Header Name:	config.h
 * Description:	Configuration Header (Default)
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#define FE_NAMESPACE			"fe"
#define BASE_NAMESPACE			"base"
#define IRC_NAMESPACE			"irc"
#define VARIABLE_PATH			"irc;base"

/* Frontend Values */
#define FE_STATUS			"fe.status"
#define FE_STATUS_BAR			" $time $irc.current_nick on $irc.current_channel"
#define FE_STATUS_BAR_HEIGHT		1

/* Base Module Values */
#define BASE_TIME			"%H:%M"
#define BASE_DATE			"%d-%b"
#define BASE_TIMESTAMP			"%d-%b:%H:%M"
#define BASE_DEFAULT_SET_TYPE		"string"

/* IRC Module Values */
//#define IRC_DEFAULT_SERVER		"irc.othersideirc.net"
//#define IRC_DEFAULT_SERVER		"192.168.1.101"
//#define IRC_DEFAULT_SERVER		"euro.othersideirc.net"
//#define IRC_DEFAULT_SERVER		"irc.neovanglist.net"
#define IRC_DEFAULT_SERVER		"clarke.freenode.net"
//#define IRC_DEFAULT_SERVER		"irc.newnet.net"
#define IRC_DEFAULT_PORT		6667
#define IRC_DEFAULT_NICK		"CaptainPants"

#define IRC_LOG_SIZE			100
#define IRC_MAX_NICK			20
#define IRC_MAX_STRING			512

#define IRC_CHANNEL_BITFLAGS		0x01

#define IRC_TIMESTAMP			"%d-%b:%H:%M"
#define IRC_BANNER			"***"

#define IRC_FMT_DEFAULT			"%N!%S %C %M"
#define IRC_FMT_ACTION			"[%@] * %N %M"
#define IRC_FMT_ACTION_SELF		"\x02[\x02%@\x02]\x02 * %n %M"
#define IRC_FMT_JOIN			"%B [%@] Joined %P1: %N (%S)"
#define IRC_FMT_KICK			"%B [%@] %P2 was kicked from %P1 by %N (%M)"
#define IRC_FMT_KICK_SELF		"%B [%@] You were kicked from %P1 by %N (%M)"
//#define IRC_FMT_NAMES			"$banner Users on <channel>%P3</channel>: <msg>%T</msg>"
#define IRC_FMT_NAMES			"%B Users on %P3: %M"
#define IRC_FMT_NICK			"%B [%@] nick: %N -> %P1"
#define IRC_FMT_NOTICE			"%B [%@] -%N- %M"
#define IRC_FMT_NOTICE_SELF		"%B \x02[\x02%@\x02]\x02 -%n- %M"
#define IRC_FMT_PART			"%B [%@] Leaving %P1: %N (%M)"
#define IRC_FMT_PRIVATE_MSG		"-[%@ %N]- %M"
#define IRC_FMT_PRIVATE_MSG_SELF	"-\x02[\x02%@ %n\x02]\x02- %M"
#define IRC_FMT_PUBLIC_MSG		"[%@ %N] %M"
#define IRC_FMT_PUBLIC_MSG_SELF		"\x02[\x02%@ %n\x02]\x02 %M"
#define IRC_FMT_QUIT			"%B [%@] Quit: %N (%M)"
#define IRC_FMT_TOPIC			"%B Topic Set by %N To: %M"

#define IRC_QUIT_MSG			"The Pooper Scooper Of Life!"

/* General Values */
#define UNKNOWN_COMMAND			"*** Unknown Command"
#define COMMAND_PREFIX			"/"
#define DEFAULT_COMMAND			""
#define NAME_SEPARATOR			'.'

#define STRING_SIZE			512

#define DOT_FILE			"~/.stutterrc"

//#define GREET_MSG			"Welcome to the show!"

#define LOAD_MODULES()			\
	init_base();			\
	init_irc();

#define RELEASE_MODULES()		\
	release_irc();			\
	release_base();

#define BIND_KEYS()			\
	BIND_KEY("\n", "parse")		\
	BIND_KEY("\x18", "next")	\
	BIND_KEY("\x15", "clearline")	\
	BIND_KEY("\x11", "previous")

#define BASE_COMMANDS()			\
	DECLARE_COMMAND(bind)		\
	DECLARE_COMMAND(clear)		\
	DECLARE_COMMAND(clearline)	\
	DECLARE_COMMAND(echo)		\
	DECLARE_COMMAND(get)		\
	DECLARE_COMMAND(next)		\
	DECLARE_COMMAND(parse)		\
	DECLARE_COMMAND(previous)	\
	DECLARE_COMMAND(quit)		\
	DECLARE_COMMAND(remove)		\
	DECLARE_COMMAND(scroll)		\
	DECLARE_COMMAND(set)		\
	DECLARE_COMMAND(source)

#define IRC_HANDLERS()							\
	ADD_HANDLER("irc_msg_dispatch", NULL, irc_dispatch_msg, NULL)	\
	ADD_HANDLER("quit", NULL, irc_sig_quit, NULL)

#define IRC_COMMANDS()			\
	DECLARE_COMMAND(disconnect)	\
	DECLARE_COMMAND(join)		\
	DECLARE_COMMAND(leave)		\
	DECLARE_COMMAND(me)		\
	DECLARE_COMMAND(msg)		\
	DECLARE_COMMAND(names)		\
	DECLARE_COMMAND(nick)		\
	DECLARE_COMMAND(notice)		\
	DECLARE_COMMAND(ping)		\
	DECLARE_COMMAND(say)		\
	DECLARE_COMMAND(server)		\
	DECLARE_COMMAND(whois)		\
	ADD_COMMAND("", say, NULL)


#define STUTTER_INIT(argc, argv) {				\
	struct variable_s *var;					\
	if (var = find_variable(NULL, "source"))		\
		var->type->evaluate(var->value, DOT_FILE);	\
}

#endif


