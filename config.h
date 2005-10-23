/*
 * Header Name:	config.h
 * Description:	Configuration Header (Default)
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#define BASE_NAMESPACE			"base"

#define IRC_NAMESPACE			"irc"

#define IRC_DEFAULT_SERVER		"euro.othersideirc.net"
//#define IRC_DEFAULT_SERVER		"clarke.freenode.net"
#define IRC_DEFAULT_PORT		6667
#define IRC_DEFAULT_NICK		"ted"

#define IRC_LOG_SIZE			100
#define IRC_MAX_NICK			20
#define IRC_MAX_STRING			512

#define IRC_CHANNEL_BITFLAGS		0x01

#define IRC_STATUS_BAR			" %H %n on %c (%t)"
#define IRC_STATUS_COLOUR		SC_INVERSE

#define IRC_TIMESTAMP			"%d-%b:%H:%M"
#define IRC_BANNER			"***"

#define IRC_FMT_DEFAULT			"%N!%S %C %M"
#define IRC_FMT_ACTION			"[%@] * %N %M"
#define IRC_FMT_ACTION_SELF		"[%@] * %n %M"
//#define IRC_FMT_PRIVMSG		"&timestamp; &msg;"
#define IRC_FMT_PRIVMSG			"[%@ %N] %M"
#define IRC_FMT_PRIVMSG_SELF		"[%@ %n] %M"
#define IRC_FMT_NOTICE			"%B [%@] -%N- %M"
#define IRC_FMT_NOTICE_SELF		"%B [%@] -%n- %M"
#define IRC_FMT_JOIN			"%B [%@] Joined %P1: %N (%S)"
#define IRC_FMT_PART			"%B [%@] Leaving %P1: %N (%M)"
#define IRC_FMT_QUIT			"%B [%@] Quit: %N (%M)"
#define IRC_FMT_TOPIC			"%B Topic Set To: %M"
//#define IRC_FMT_NAMES			"&banner; Users on <channel>%P3</channel>: &msg;"
#define IRC_FMT_NAMES			"%B Users on %P3: %M"
#define IRC_FMT_KICK			"%B [%@] %P2 was kicked from %P1 by %N (%M)"
#define IRC_FMT_KICK_SELF		"%B [%@] You were kicked from %P1 by %N (%M)"
#define IRC_FMT_NICK			"%B [%@] nick: %N -> %P1"

#define IRC_QUIT_MSG			"The Pooper Scooper Of Life!"

/** Name of function to callback when a msg is received or NULL to disable irc (default: base_msg_dispatch) */
#define DISPATCH_CALLBACK		irc_msg_dispatch

#define UNKNOWN_COMMAND			"*** Unknown Command"
#define COMMAND_PREFIX			"/"
#define DEFAULT_COMMAND			""
#define COMMAND_PARSER			base_cmd_parse
	
//#define GREET_MSG			"Welcome to the show!"

#define DOTFILE_DIR			".stutter"
#define DEFAULT_DOTFILE_DIR		"/usr/local/etc"

#endif


