/*
 * Header Name:	config.h
 * Description:	Configuration Header (Default)
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef BUILTIN_CONFIG
	#define DECLARE_CONFIG(name, value)	// save value?!??
	#define CONFIG_VALUE(name)		name
#else

#endif

#define FE_NAMESPACE			"fe"
#define BASE_NAMESPACE			"base"
#define IRC_NAMESPACE			"irc"
#define VARIABLE_PATH			"irc;base"

/* Frontend Values */
#define FE_STATUS			"status"
#define FE_STATUS_BAR			" $time $irc:current_nick on $irc:current_channel"
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
#define IRC_DEFAULT_SERVER		"irc.neovanglist.net"
//#define IRC_DEFAULT_SERVER		"clarke.freenode.net"
#define IRC_DEFAULT_PORT		6667
#define IRC_DEFAULT_NICK		"ted"

#define IRC_LOG_SIZE			100
#define IRC_MAX_NICK			20
#define IRC_MAX_STRING			512

#define IRC_CHANNEL_BITFLAGS		0x01

#define IRC_TIMESTAMP			"%d-%b:%H:%M"
#define IRC_BANNER			"***"

#define IRC_FMT_DEFAULT			"%N!%S %C %M"
#define IRC_FMT_ACTION			"[%@] * %N %M"
#define IRC_FMT_ACTION_SELF		"[%@] * %n %M"
#define IRC_FMT_PRIVMSG			"[%@ %N] %M"
#define IRC_FMT_PRIVMSG_SELF		"[%@ %n] %M"
#define IRC_FMT_NOTICE			"%B [%@] -%N- %M"
#define IRC_FMT_NOTICE_SELF		"%B [%@] -%n- %M"
#define IRC_FMT_JOIN			"%B [%@] Joined %P1: %N (%S)"
#define IRC_FMT_PART			"%B [%@] Leaving %P1: %N (%M)"
#define IRC_FMT_QUIT			"%B [%@] Quit: %N (%M)"
#define IRC_FMT_TOPIC			"%B Topic Set by %N To: %M"
//#define IRC_FMT_NAMES			"$banner Users on <channel>%P3</channel>: <msg>%T</msg>"
#define IRC_FMT_NAMES			"%B Users on %P3: %M"
#define IRC_FMT_KICK			"%B [%@] %P2 was kicked from %P1 by %N (%M)"
#define IRC_FMT_KICK_SELF		"%B [%@] You were kicked from %P1 by %N (%M)"
#define IRC_FMT_NICK			"%B [%@] nick: %N -> %P1"

#define IRC_QUIT_MSG			"The Pooper Scooper Of Life!"

/* General Values */
#define UNKNOWN_COMMAND			"*** Unknown Command"
#define COMMAND_PREFIX			"/"
#define DEFAULT_COMMAND			""
	
//#define GREET_MSG			"Welcome to the show!"

#define DOTFILE_DIR			".stutter"
#define DEFAULT_DOTFILE_DIR		"/usr/local/etc"



//#define IRC_CONFIG()						\
//	DECLARE_CONFIG(IRC_FMT_DEFAULT, "%N!%S %C %M")		\


// GET_CONFIG(IRC_FMT_DEFAULT)
// TODO the problem with this is that we either make all configs vars or all compiled in

#endif


