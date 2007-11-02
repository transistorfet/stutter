/*
 * Automatically generated configuration
 */

#ifndef _CONFIG_H
#define _CONFIG_H

/** Modules */
#include <stutter/modules/base.h>
#include <stutter/modules/irc.h>

#define INIT_MODULES()	\
	init_base();	\
	init_irc();	\

#define RELEASE_MODULES()	\
	release_base();	\
	release_irc();	\


/** Configuration Options */
#define CONFIG_BASE_ALIAS
#define CONFIG_BASE_BIND
#define CONFIG_BASE_CLEAR
#define CONFIG_BASE_CLEARLINE
#define CONFIG_BASE_CLOSE
#define CONFIG_BASE_CONCAT
#define CONFIG_BASE_CONTEXT
#define CONFIG_BASE_ECHO
#define CONFIG_BASE_EVALUATE
#define CONFIG_BASE_EXEC
#define CONFIG_BASE_EXPAND
#define CONFIG_BASE_GET
#define CONFIG_BASE_NORMAL
#define CONFIG_BASE_PARSE
#define CONFIG_BASE_QUIT
#define CONFIG_BASE_REDIRECT
#define CONFIG_BASE_REMOVE
#define CONFIG_BASE_SCROLL
#define CONFIG_BASE_SELECT
#define CONFIG_BASE_SET
#define CONFIG_BASE_SOURCE
#define CONFIG_BASE_TIMER
#define CONFIG_BASE_UNBIND
#define CONFIG_BASE_WINDOW
#define CONFIG_FE_HASHATTRIB
#define CONFIG_FE_INSERT
#define CONFIG_FE_NEXT
#define CONFIG_FE_PREVIOUS
#define CONFIG_IRC_ACTIVATE
#define CONFIG_IRC_ALLCOMMANDS
#define CONFIG_IRC_AUTOCOMPLETE
#define CONFIG_IRC_COLOURED_NICKS
#define CONFIG_IRC_COMPLETE
#define CONFIG_IRC_CTCP
#define CONFIG_IRC_DISCONNECT
#define CONFIG_IRC_ENDQUERY
#define CONFIG_IRC_JOIN
#define CONFIG_IRC_KICK
#define CONFIG_IRC_LEAVE
#define CONFIG_IRC_ME
#define CONFIG_IRC_MODE
#define CONFIG_IRC_MSG
#define CONFIG_IRC_NAMES
#define CONFIG_IRC_NICK
#define CONFIG_IRC_NORMAL
#define CONFIG_IRC_NOTICE
#define CONFIG_IRC_PART
#define CONFIG_IRC_PING
#define CONFIG_IRC_QUERY
#define CONFIG_IRC_RECONNECT
#define CONFIG_IRC_SAY
#define CONFIG_IRC_SERVER
#define CONFIG_IRC_TOPIC
#define CONFIG_IRC_WHOIS
#define CONFIG_IRC_WHOWAS
#define CONFIG_NORMAL


/** Definitions */
#define BASE_COMMAND_PREFIX                      "/"
#define BASE_ERR_ALIAS_FAILED                    "\022error\022Error creating alias %s"
#define BASE_ERR_BINDING_FAILED                  "\022error\022Error binding key %s"
#define BASE_ERR_EVALUATE_FAILED                 "\022error\022Error evaluating variable %s"
#define BASE_ERR_REDIRECT_FAILED                 "\022error\022Error: Invalid target location %s"
#define BASE_ERR_REMOVE_FAILED                   "\022error\022Error removing variable %s"
#define BASE_ERR_SET_FAILED                      "\022error\022Error setting variable %s"
#define BASE_ERR_STRINGIFY_FAILED                "\022error\022Error stringifying variable %s"
#define BASE_ERR_TYPE_NOT_FOUND                  "\022error\022Error: %s type not found"
#define BASE_ERR_UNABLE_TO_OPEN_FILE             "\022error\022Error: Unable to open the file %s"
#define BASE_ERR_UNBINDING_FAILED                "\022error\022Error unbinding key %s"
#define BASE_ERR_UNKNOWN_COMMAND                 "\022error\022Error: Unknown Command %s."
#define BASE_ERR_VARIABLE_NOT_FOUND              "\022error\022Error: %s variable not found"
#define BASE_FMT_ALIAS                           "Alias: %s <= %s"
#define BASE_FMT_BIND                            "Key %s bound to %s %s"
#define BASE_FMT_REDIRECT_CURRENT                "Signal %s is directed to the current window"
#define BASE_FMT_REDIRECT_DISCONNECTED           "Signal %s has been disconnected"
#define BASE_FMT_REDIRECT_THIS                   "Signal %s is directed to this window"
#define BASE_FMT_REMOVE                          "Variable %s removed"
#define BASE_FMT_SET                             "Variable: %s <= %s"
#define BASE_FMT_UNBIND                          "Key %s is not longer bound"
#define BASE_NAMESPACE                           "base"
#define BASE_NULL_COMMAND                        ""
#define FE_COLOURMAP_SIZE                        32
#define FE_DEFAULT_STATUSBAR                     "$&fe.statusbar"
#define FE_FORMAT_MAX_STYLES                     64
#define FE_INPUT_BUFFER_SIZE                     1024
#define FE_INPUT_HISTORY_SIZE                    200
#define FE_LAYOUT_TYPES_GROWTH_FACTOR            1.75
#define FE_LAYOUT_TYPES_INIT_SIZE                20
#define FE_LAYOUT_TYPES_LOAD_FACTOR              0.75
#define FE_LAYOUTS_GROWTH_FACTOR                 1.75
#define FE_LAYOUTS_INIT_SIZE                     20
#define FE_LAYOUTS_LOAD_FACTOR                   0.75
#define FE_NET_ERR_FAILED_TO_CONNECT             "\022error\022Error: Failed to connect to %s"
#define FE_NET_ERR_RETRYING                      "\022error\022Error: Failed to connect to %s, Retrying..."
#define FE_STATUS_BAR_HEIGHT                     1
#define FE_SURFACE_LIST_GROWTH_FACTOR            1.75
#define FE_SURFACE_LIST_INIT_SIZE                20
#define FE_SURFACE_LIST_LOAD_FACTOR              0.75
#define FE_TEXT_LOG_SIZE                         1000
#define FE_TEXT_WRAP_STRING                      "        "
#define FE_WIDGET_LIST_GROWTH_FACTOR             1.75
#define FE_WIDGET_LIST_INIT_SIZE                 20
#define FE_WIDGET_LIST_LOAD_FACTOR               0.75
#define IRC_BANNER                               "***"
#define IRC_CHANNEL_BITFLAGS                     0x01
#define IRC_DEFAULT_NICK                         "CaptainPants"
#define IRC_DEFAULT_PORT                         6667
#define IRC_ERR_JOIN_ERROR                       "\022error\022Error: Unable to create channel resources after joining %s"
#define IRC_ERR_MSG_NOT_ENOUGH_PARAMS            "\022error\022Error: IRC message, %s, doesn't have enough parameters"
#define IRC_ERR_MSG_TOO_MANY_PARAMS              "\022error\022Error: IRC message, %s, has too many parameters"
#define IRC_ERR_QUERY_ERROR                      "\022error\022Error: Unable to create query resources for %s"
#define IRC_ERR_RECONNECT_ERROR                  "\022error\022Error: Unable to reconnect to %s"
#define IRC_ERR_SERVER_DISCONNECTED              "\022error\022Error: Disconnected from %s"
#define IRC_FMT_CTCP_UNKNOWN                     "\022status\022%B [\022timestamp\022%@\022status\022] Unknown CTCP %T0 received from \022nick\022%N\022status\022 (%H)"
#define IRC_FMT_CTCP_VERSION                     "\022status\022%B [\022timestamp\022%@\022status\022] CTCP Version received from \022nick\022%N\022status\022 (%H)"
#define IRC_FMT_CTCP_VERSION_REPLY               "\022status\022%B [\022timestamp\022%@\022status\022] CTCP Version reply from \022nick\022%N\022status\022 (%H) %A0"
#define IRC_FMT_CURRENT                          "%N!%H %C %M"
#define IRC_FMT_DEFAULT                          "%N!%H %C %M"
#define IRC_FMT_JOIN                             "\022status\022%B [\022timestamp\022%@\022status\022] Joined \022channel\022%P1\022status\022: \022nick\022%N\022status\022 (%H)"
#define IRC_FMT_KICK                             "\022status\022%B [\022timestamp\022%@\022status\022] \022nick\022%P2\022status\022 was kicked from \022channel\022%P1\022status\022 by \022nick\022%N\022status\022 (%M)"
#define IRC_FMT_KICK_SELF                        "\022status\022%B [\022timestamp\022%@\022status\022] You were kicked from \022channel\022%P1\022status\022 by \022nick\022%N\022status\022 (%M)"
#define IRC_FMT_MODE                             "\022status\022%B Mode on \022channel\022%P1\022status\022 by \022nick\022%N\022status\022: %P2 %P3 %P4 %P5 %P6 %P7 %P8"
#define IRC_FMT_NAMES                            "\022status\022%B Users on \022channel\022%P3\022status\022: %M"
#define IRC_FMT_NICK                             "\022status\022%B [\022timestamp\022%@\022status\022] nick: \022nick\022%N\022status\022 -> \022nick\022%P1"
#define IRC_FMT_NICKNAMEINUSE                    "\022error\022%B [\022timestamp\022%@\022error\022] \022nick\022%P2: \022error\022%M"
#define IRC_FMT_NOSUCHNICK                       "\022error\022%B [\022timestamp\022%@\022error\022] \022nick\022%P2: \022error\022%M"
#define IRC_FMT_NOTICE                           "%B [\022timestamp\022%@\022default\022] -\022nick\022%N\022default\022- \022message\022%M"
#define IRC_FMT_NOTICE_SELF                      "%B \022bracket\022[\022timestamp\022%@\022bracket\022]\022default\022 -\022nick\022%n\022default\022- \022message\022%M"
#define IRC_FMT_PART                             "\022status\022%B [\022timestamp\022%@\022status\022] Leaving \022channel\022%P1\022status\022: \022nick\022%N\022status\022 (%M)"
#define IRC_FMT_PING                             "\022status\022%B [\022timestamp\022%@\022status\022] Ping received from \022nick\022%N\022status\022 (%H)"
#define IRC_FMT_PING_REPLY                       "\022status\022%B [\022timestamp\022%@\022status\022] Ping reply from \022nick\022%N\022status\022 (%A0)"
#define IRC_FMT_PRIVATE_ACTION                   "[\022timestamp\022%@\022default\022] <*> \022nicktable.%N\022%N\022default\022 %A0"
#define IRC_FMT_PRIVATE_ACTION_SELF              "\022bracket\022[\022timestamp\022%@\022bracket\022]\022default\022 <*> \022nick\022%n\022message\022 %A0"
#define IRC_FMT_PRIVATE_MSG                      "-[\022timestamp\022%@ \022nicktable.%N\022%N\022default\022]- \022message\022%M"
#define IRC_FMT_PRIVATE_MSG_SELF                 "-\022bracket\022[\022timestamp\022%@ \022nick\022%n\022default\022 -> \022nick\022%P1\022bracket\022]\022default\022- \022message\022%M"
#define IRC_FMT_PUBLIC_ACTION                    "[\022timestamp\022%@\022default\022] * \022nicktable.%N\022%N\022message\022 %A0"
#define IRC_FMT_PUBLIC_ACTION_SELF               "\022bracket\022[\022nick\022%@\022bracket\022]\022default\022 * \022nick\022%n\022message\022 %A0"
#define IRC_FMT_PUBLIC_MSG                       "[\022timestamp\022%@ \022nicktable.%N\022%N\022default\022] \022message\022%M"
#define IRC_FMT_PUBLIC_MSG_SELF                  "\022bracket\022[\022timestamp\022%@ \022nick\022%n\022bracket\022]\022message\022 %M"
#define IRC_FMT_QUIT                             "\022status\022%B [\022timestamp\022%@\022status\022] Quit: \022nick\022%N\022status\022 (%M)"
#define IRC_FMT_STATUS                           "\022status\022%B [\022timestamp\022%@\022status\022] %M"
#define IRC_FMT_TOPIC                            "\022topic\022%B Topic Set by \022nick\022%N\022topic\022 To: %M"
#define IRC_FMT_WHOISCHANNELS                    "\022status\022%B [Whois \022nick\022%P2\022status\022] Channels : \022channel\022%M"
#define IRC_FMT_WHOISIDLE                        "\022status\022%B [Whois \022nick\022%P2\022status\022] %P3 %M"
#define IRC_FMT_WHOISOPERATOR                    "\022status\022%B \022nick\022%P2\022status\022 %M"
#define IRC_FMT_WHOISSERVER                      "\022status\022%B [Whois \022nick\022%P2\022status\022] Server : \022server\022%P3\022status\022 (%M)"
#define IRC_FMT_WHOISSPECIAL                     "\022status\022%B %M"
#define IRC_FMT_WHOISUSER                        "\022status\022%B [Whois \022nick\022%P2\022status\022] Realname : %M"
#define IRC_FMT_WHOWASUSER                       "\022status\022%B [Whowas \022nick\022%P2\022status\022 (%P3@%P4)] Realname : %M"
#define IRC_LOG_SIZE                             100
#define IRC_MAX_STRING                           512
#define IRC_OUT_ATTEMPTING_RECONNECT             "\022status\022Attempting to reconnect in %d seconds"
#define IRC_PING_WATCHDOG_TIMEOUT                300
#define IRC_QUIT_MSG                             "The Pooper Scooper Of Life!"
#define IRC_RECONNECT_RETRIES                    0
#define IRC_RETRY_DELAY                          60
#define IRC_SERVER_DISCONNECTED                  "\022status\022Disconnected from %s"
#define IRC_TIMESTAMP                            "%d-%b:%H:%M"
#define IRC_VERSION_RESPONSE                     "St-St-Stutter " STUTTER_VERSION
#define KEY_CONTEXT_LIST_INIT_SIZE               10
#define KEY_MAP_GROWTH_FACTOR                    1.75
#define KEY_MAP_LOAD_FACTOR                      0.75
#define KEY_ROOTMAP_INIT_SIZE                    20
#define KEY_SUBMAP_INIT_SIZE                     20
#define LARGE_STRING_SIZE                        1024
#define NAME_SEPARATOR                           '.'
#define NAME_STRING_SIZE                         64
#define PATH_VARIABLE_NAME                       "path"
#define SIGNAL_LIST_GROWTH_FACTOR                1.75
#define SIGNAL_LIST_INIT_SIZE                    20
#define SIGNAL_LIST_LOAD_FACTOR                  0.75
#define SIGNAL_OBJECTS_GROWTH_FACTOR             1.75
#define SIGNAL_OBJECTS_INIT_SIZE                 20
#define SIGNAL_OBJECTS_LOAD_FACTOR               0.75
#define SMALL_STRING_SIZE                        128
#define STRING_SIZE                              512
#define STUTTER_VERSION                          "0.4-alpha"
#define TYPE_LIST_GROWTH_FACTOR                  1.75
#define TYPE_LIST_INIT_SIZE                      20
#define TYPE_LIST_LOAD_FACTOR                    0.75
#define VARIABLE_LIST_GROWTH_FACTOR              1.75
#define VARIABLE_LIST_INIT_SIZE                  20
#define VARIABLE_LIST_LOAD_FACTOR                0.75


#define BASE_COMMAND_RESPONSE_JOINPOINT(...) {	\
	BASE_OUTPUT_JOINPOINT(__VA_ARGS__);	\
}


#define BASE_ERROR_JOINPOINT(...) {	\
	ERROR_JOINPOINT("base.error", __VA_ARGS__);	\
}


#define BASE_INIT_JOINPOINT() {	\
}


#define BASE_OUTPUT_JOINPOINT(...) {	\
	OUTPUT_JOINPOINT("base.output", __VA_ARGS__);	\
}


#define BASE_RELEASE_JOINPOINT() {	\
}


#define ERROR_JOINPOINT(name, ...) {	\
	PRINT_TO_CURRENT(__VA_ARGS__);	\
}


#define FE_NET_ERROR_JOINPOINT(...) {	\
	ERROR_JOINPOINT("fe.error", __VA_ARGS__);	\
}


#define ADD_HANDLERS()	\
	ADD_HANDLER(NULL, "base.error", 10, base_sig_print, NULL)	\
	ADD_HANDLER(NULL, "base.exec_output", 10, irc_sig_exec, NULL)	\
	ADD_HANDLER(NULL, "error_general", 10, base_sig_print, NULL)	\
	ADD_HANDLER(NULL, "fe.quit", 10, irc_sig_quit, NULL)	\
	ADD_HANDLER(NULL, "irc.dispatch_ctcp", 10, irc_sig_dispatch_ctcp_msg, NULL)	\
	ADD_HANDLER(NULL, "irc.dispatch_msg", 10, irc_sig_dispatch_msg, NULL)	\


#define ADD_INITS()	\
	EVALUATE_COMMAND("source ~/.stutterrc")	\
	EVALUATE_COMMAND("base.alias setcolour set fe.theme.")	\
	EVALUATE_COMMAND("base.alias getcolour get fe.theme.")	\


#define IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_PRINT_TO_CHANNEL(channel, msg, fmt);	\
}


#define IRC_ERROR_JOINPOINT(...) {	\
	ERROR_JOINPOINT("irc.error", __VA_ARGS__);	\
}


#define IRC_INIT_JOINPOINT() {	\
}


#define IRC_MSG_CTCP_ACTION_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\
}


#define IRC_MSG_CTCP_PING_OUTPUT_JOINPOINT(msg, fmt) {	\
	IRC_PRINT_TO_CURRENT_AND_STATUS(msg, fmt);	\
}


#define IRC_MSG_CTCP_UNKNOWN_OUTPUT_JOINPOINT(msg, fmt) {	\
	IRC_PRINT_TO_CURRENT(msg, fmt);	\
}


#define IRC_MSG_CTCP_VERSION_OUTPUT_JOINPOINT(msg, fmt) {	\
	IRC_PRINT_TO_CURRENT_AND_STATUS(msg, fmt);	\
}


#define IRC_MSG_CURRENT_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\
}


#define IRC_MSG_DEFAULT_OUTPUT_JOINPOINT(msg, fmt) {	\
	IRC_SERVER_OUTPUT_JOINPOINT(msg->server, msg, fmt);	\
}


#define IRC_MSG_INUSE_OUTPUT_JOINPOINT(msg, fmt) {	\
	IRC_PRINT_TO_CURRENT(msg, fmt);	\
}


#define IRC_MSG_JOIN_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\
}


#define IRC_MSG_KICK_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\
}


#define IRC_MSG_MODE_OUTPUT_JOINPOINT(channel, msg, fmt)	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\


#define IRC_MSG_NAMES_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\
}


#define IRC_MSG_NICK_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\
}


#define IRC_MSG_NOTICE_OUTPUT_JOINPOINT(msg, fmt) {	\
	IRC_PRINT_TO_CURRENT_AND_STATUS(msg, fmt);	\
}


#define IRC_MSG_PART_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\
}


#define IRC_MSG_PRIVMSG_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\
}


#define IRC_MSG_QUIT_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\
}


#define IRC_MSG_TOPIC_OUTPUT_JOINPOINT(channel, msg, fmt) {	\
	IRC_CHANNEL_OUTPUT_JOINPOINT(channel, msg, fmt);	\
}


#define IRC_OUTPUT_JOINPOINT(...) {	\
	OUTPUT_JOINPOINT("irc.output", __VA_ARGS__);	\
}


#define IRC_RELEASE_JOINPOINT() {	\
}


#define IRC_SERVER_OUTPUT_JOINPOINT(server, msg, fmt) {	\
	IRC_PRINT_TO_STATUS(server, msg, fmt);	\
}


#define OUTPUT_JOINPOINT(name, ...) {	\
	PRINT_TO_CURRENT(__VA_ARGS__);	\
}


#define SYSTEM_INIT_JOINPOINT() {	\
}


#define SYSTEM_RELEASE_JOINPOINT() {	\
}


#define ADD_TABLES()	\
	ADD_FIXED_VARIABLE("base", "")	\
	ADD_FIXED_VARIABLE("fe", "")	\
	ADD_FIXED_VARIABLE("fe.theme", "")	\
	ADD_FIXED_VARIABLE("irc", "")	\


#define ADD_TYPES()	\
	ADD_TYPE(irc_load_channel_list)	\
	ADD_TYPE(irc_load_channel_type)	\
	ADD_TYPE(fe_common_load_colour)	\
	ADD_TYPE(base_load_command)	\
	ADD_TYPE(fe_common_load_command)	\
	ADD_TYPE(base_load_format)	\
	ADD_TYPE(irc_load_server_list)	\
	ADD_TYPE(irc_load_server_type)	\
	ADD_TYPE(base_load_status)	\
	ADD_TYPE(base_load_string)	\
	ADD_TYPE(base_load_string_ref)	\
	ADD_TYPE(fe_common_load_style)	\
	ADD_TYPE(base_load_table)	\
	ADD_TYPE(base_load_time)	\
	ADD_TYPE(base_load_timer)	\
	ADD_TYPE(irc_load_user_list)	\
	ADD_TYPE(irc_load_user_type)	\
	ADD_TYPE(base_load_wildcard)	\
	ADD_TYPE(fe_common_load_attrib)	\
	ADD_TYPE(fe_common_load_hashattrib)	\


/** Key Bindings */
#define ADD_BINDINGS()	\
	BIND_KEY("\n", "base.parse", "irc.activate")	\
	BIND_KEY("\t", "irc.complete", NULL)	\
	BIND_KEY("^c^x", "base.quit", NULL)	\
	BIND_KEY("^k", "fe.insert", "\x03")	\
	BIND_KEY("^p", "base.scroll", "1")	\
	BIND_KEY("^q", "fe.previous", NULL)	\
	BIND_KEY("^u", "base.clearline", NULL)	\
	BIND_KEY("^x", "fe.next", NULL)	\
	BIND_KEY("^y", "base.scroll", "-1")	\


/** Variables */
#define ADD_VARIABLES()	\
	DECLARE_TYPE("command",	\
		ADD_COMMAND_ENV("base.alias", base_cmd_alias, NULL)	\
		ADD_COMMAND_ENV("base.bind", base_cmd_bind, NULL)	\
		ADD_COMMAND_ENV("base.clear", base_cmd_clear, NULL)	\
		ADD_COMMAND_ENV("base.clearline", base_cmd_clearline, NULL)	\
		ADD_COMMAND_ENV("base.close", base_cmd_close, NULL)	\
		ADD_COMMAND_ENV("base.concat", base_cmd_concat, NULL)	\
		ADD_COMMAND_ENV("base.context", base_cmd_context, NULL)	\
		ADD_COMMAND_ENV("base.echo", base_cmd_echo, NULL)	\
		ADD_COMMAND_ENV("base.evaluate", base_cmd_evaluate, NULL)	\
		ADD_COMMAND_ENV("base.exec", base_cmd_exec, NULL)	\
		ADD_COMMAND_ENV("base.expand", base_cmd_expand, NULL)	\
		ADD_COMMAND_ENV("base.get", base_cmd_get, NULL)	\
		ADD_COMMAND_ENV("base.parse", base_cmd_parse, NULL)	\
		ADD_COMMAND_ENV("base.quit", base_cmd_quit, NULL)	\
		ADD_COMMAND_ENV("base.redirect", base_cmd_redirect, NULL)	\
		ADD_COMMAND_ENV("base.remove", base_cmd_remove, NULL)	\
		ADD_COMMAND_ENV("base.scroll", base_cmd_scroll, NULL)	\
		ADD_COMMAND_ENV("base.select", base_cmd_select, NULL)	\
		ADD_COMMAND_ENV("base.set", base_cmd_set, NULL)	\
		ADD_COMMAND_ENV("base.source", base_cmd_source, NULL)	\
		ADD_COMMAND_ENV("base.timer", base_cmd_timer, NULL)	\
		ADD_COMMAND_ENV("base.unbind", base_cmd_unbind, NULL)	\
		ADD_COMMAND_ENV("base.window", base_cmd_window, NULL)	\
		ADD_COMMAND_ENV("irc.activate", base_cmd_evaluate, "irc.say")	\
		ADD_COMMAND_ENV("irc.complete", irc_cmd_complete, NULL)	\
		ADD_COMMAND_ENV("irc.ctcp", irc_cmd_ctcp, NULL)	\
		ADD_COMMAND_ENV("irc.disconnect", irc_cmd_disconnect, NULL)	\
		ADD_COMMAND_ENV("irc.endquery", irc_cmd_endquery, NULL)	\
		ADD_COMMAND_ENV("irc.join", irc_cmd_join, NULL)	\
		ADD_COMMAND_ENV("irc.kick", irc_cmd_kick, NULL)	\
		ADD_COMMAND_ENV("irc.leave", irc_cmd_leave, NULL)	\
		ADD_COMMAND_ENV("irc.me", irc_cmd_me, NULL)	\
		ADD_COMMAND_ENV("irc.mode", irc_cmd_mode, NULL)	\
		ADD_COMMAND_ENV("irc.msg", irc_cmd_msg, NULL)	\
		ADD_COMMAND_ENV("irc.names", irc_cmd_names, NULL)	\
		ADD_COMMAND_ENV("irc.nick", irc_cmd_nick, NULL)	\
		ADD_COMMAND_ENV("irc.notice", irc_cmd_notice, NULL)	\
		ADD_COMMAND_ENV("irc.part", irc_cmd_leave, NULL)	\
		ADD_COMMAND_ENV("irc.ping", irc_cmd_ping, NULL)	\
		ADD_COMMAND_ENV("irc.query", irc_cmd_query, NULL)	\
		ADD_COMMAND_ENV("irc.reconnect", irc_cmd_reconnect, NULL)	\
		ADD_COMMAND_ENV("irc.say", irc_cmd_say, NULL)	\
		ADD_COMMAND_ENV("irc.server", irc_cmd_server, NULL)	\
		ADD_COMMAND_ENV("irc.topic", irc_cmd_topic, NULL)	\
		ADD_COMMAND_ENV("irc.whois", irc_cmd_whois, NULL)	\
		ADD_COMMAND_ENV("irc.whowas", irc_cmd_whowas, NULL)	\
	)	\
	DECLARE_TYPE("command:fe",	\
		ADD_FIXED_VARIABLE("fe.insert", "callback,widget", fe_common_cmd_insert, NULL)	\
		ADD_FIXED_VARIABLE("fe.next", "callback,widget", fe_common_cmd_next, NULL)	\
		ADD_FIXED_VARIABLE("fe.previous", "callback,widget", fe_common_cmd_previous, NULL)	\
	)	\
	DECLARE_TYPE("hashattrib:fe",	\
		ADD_VARIABLE("fe.theme.nicktable", "", "")	\
	)	\
	DECLARE_TYPE("status",	\
		ADD_VARIABLE("irc.current_channel", "stringify,pointer", irc_stringify_channel, NULL)	\
		ADD_VARIABLE("irc.current_nick", "stringify,pointer", irc_stringify_nick, NULL)	\
	)	\
	DECLARE_TYPE("attrib:fe",	\
		ADD_VARIABLE("fe.theme.bracket", "string", ",,bold")	\
		ADD_VARIABLE("fe.theme.channel", "string", "default")	\
		ADD_VARIABLE("fe.theme.default", "string", "default")	\
		ADD_VARIABLE("fe.theme.error", "string", "default")	\
		ADD_VARIABLE("fe.theme.message", "string", "default")	\
		ADD_VARIABLE("fe.theme.nick", "string", "default")	\
		ADD_VARIABLE("fe.theme.server", "string", "default")	\
		ADD_VARIABLE("fe.theme.status", "string", "default")	\
		ADD_VARIABLE("fe.theme.statusbar", "string", ",,inverse")	\
		ADD_VARIABLE("fe.theme.timestamp", "string", "default")	\
		ADD_VARIABLE("fe.theme.topic", "string", "default")	\
	)	\
	DECLARE_TYPE("format",	\
		ADD_VARIABLE("fe.statusbar", "string", "\022statusbar\022 $base.time $irc.current_nick on $irc.current_channel")	\
	)	\
	DECLARE_TYPE("string",	\
		ADD_VARIABLE("fe.type", "string", "curses")	\
	)	\
	DECLARE_TYPE("time",	\
		ADD_VARIABLE("base.date", "string", "%d-%b")	\
		ADD_VARIABLE("base.time", "string", "%H:%M")	\
		ADD_VARIABLE("base.timestamp", "string", "%d-%b:%H:%M")	\
	)	\
	DECLARE_TYPE("wildcard",	\
		ADD_VARIABLE("path", "string", ";irc;base")	\
	)	\


/** Emits */
#include <stutter/advice.h>
#include <stutter/modules/irc/advice.h>


#endif

