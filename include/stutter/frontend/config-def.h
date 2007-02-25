/*
 * Header Name:	config-def.h
 * Description:	Default Frontend Configuration Header
 */

#ifndef _FRONTEND_CONFIG_DEF_H
#define _FRONTEND_CONFIG_DEF_H

#include <stutter/config-macros.h>

#define DEFAULT_FE_STATUSBAR			"$&fe.statusbar"
#define DEFAULT_FE_STATUSBAR_DEFAULT		"\022statusbar\022 $base.time"
#define DEFAULT_FE_STATUS_BAR_HEIGHT		1
#define DEFAULT_FE_TEXT_LOG_SIZE		1000
#define DEFAULT_FE_TEXT_WRAP_STRING		"        "
#define DEFAULT_FE_INPUT_BUFFER_SIZE		1024
#define DEFAULT_FE_INPUT_HISTORY_SIZE		200

#define DEFAULT_FE_WIDGET_LIST_INIT_SIZE	20
#define DEFAULT_FE_WIDGET_LIST_LOAD_FACTOR	0.75
#define DEFAULT_FE_WIDGET_LIST_GROWTH_FACTOR	1.75

#define DEFAULT_FE_LAYOUT_TYPES_INIT_SIZE	20
#define DEFAULT_FE_LAYOUT_TYPES_LOAD_FACTOR	0.75
#define DEFAULT_FE_LAYOUT_TYPES_GROWTH_FACTOR	1.75

#define DEFAULT_FE_LAYOUTS_INIT_SIZE		20
#define DEFAULT_FE_LAYOUTS_LOAD_FACTOR		0.75
#define DEFAULT_FE_LAYOUTS_GROWTH_FACTOR	1.75

#define DEFAULT_FE_COLOURMAP_SIZE		32
#define DEFAULT_FE_FORMAT_MAX_STYLES		64

#define DEFAULT_FE_NET_ERR_RETRYING		"\022error\022Error: Failed to connect to %s, Retrying..."
#define DEFAULT_FE_NET_ERR_FAILED_TO_CONNECT	"\022error\022Error: Failed to connect to %s"

#define DEFAULT_FE_THEME_BRACKET		",,bold"
#define DEFAULT_FE_THEME_CHANNEL		"default"
#define DEFAULT_FE_THEME_DEFAULT		"default"
#define DEFAULT_FE_THEME_ERROR			"default"
#define DEFAULT_FE_THEME_MESSAGE		"default"
#define DEFAULT_FE_THEME_NICK			"default"
#define DEFAULT_FE_THEME_SERVER			"default"
#define DEFAULT_FE_THEME_STATUS			"default"
#define DEFAULT_FE_THEME_STATUSBAR		",,inverse"
#define DEFAULT_FE_THEME_TIMESTAMP		"default"
#define DEFAULT_FE_THEME_TOPIC			"default"

#define DEFAULT_FE_COMMON_ATTRIBS()						\
	ADD_FIXED_VARIABLE("theme.bracket", "string", FE_THEME_BRACKET)		\
	ADD_FIXED_VARIABLE("theme.channel", "string", FE_THEME_CHANNEL)		\
	ADD_FIXED_VARIABLE("theme.default", "string", FE_THEME_DEFAULT)		\
	ADD_FIXED_VARIABLE("theme.error", "string", FE_THEME_ERROR)		\
	ADD_FIXED_VARIABLE("theme.message", "string", FE_THEME_MESSAGE)		\
	ADD_FIXED_VARIABLE("theme.nick", "string", FE_THEME_NICK)		\
	ADD_FIXED_VARIABLE("theme.server", "string", FE_THEME_SERVER)		\
	ADD_FIXED_VARIABLE("theme.status", "string", FE_THEME_STATUS)		\
	ADD_FIXED_VARIABLE("theme.statusbar", "string", FE_THEME_STATUSBAR)	\
	ADD_FIXED_VARIABLE("theme.timestamp", "string", FE_THEME_TIMESTAMP)	\
	ADD_FIXED_VARIABLE("theme.topic", "string", FE_THEME_TOPIC)

#define DEFAULT_FE_COMMON_COMMANDS()						\
	ADD_FIXED_VARIABLE("insert", "callback,widget", fe_common_cmd_insert, NULL)

#define DEFAULT_FE_COMMON_FORMATS()						\
	ADD_FIXED_VARIABLE("statusbar", "string", FE_STATUSBAR_DEFAULT)

#define DEFAULT_FE_BINDINGS()				\
	BIND_KEY("\x0b", "fe.insert", "\x03")

/*** Joinpoints ***/

#define DEFAULT_FE_NET_ERROR_JOINPOINT(...)	\
	ERROR_JOINPOINT("fe.error", __VA_ARGS__)

/*** START OF DEFAULT ASSIGNMENTS ***/
#ifndef FE_STATUSBAR
#define FE_STATUSBAR \
	DEFAULT_FE_STATUSBAR
#endif
#ifndef FE_STATUSBAR_DEFAULT
#define FE_STATUSBAR_DEFAULT \
	DEFAULT_FE_STATUSBAR_DEFAULT
#endif
#ifndef FE_STATUS_BAR_HEIGHT
#define FE_STATUS_BAR_HEIGHT \
	DEFAULT_FE_STATUS_BAR_HEIGHT
#endif
#ifndef FE_TEXT_LOG_SIZE
#define FE_TEXT_LOG_SIZE \
	DEFAULT_FE_TEXT_LOG_SIZE
#endif
#ifndef FE_TEXT_WRAP_STRING
#define FE_TEXT_WRAP_STRING \
	DEFAULT_FE_TEXT_WRAP_STRING
#endif
#ifndef FE_INPUT_BUFFER_SIZE
#define FE_INPUT_BUFFER_SIZE \
	DEFAULT_FE_INPUT_BUFFER_SIZE
#endif
#ifndef FE_INPUT_HISTORY_SIZE
#define FE_INPUT_HISTORY_SIZE \
	DEFAULT_FE_INPUT_HISTORY_SIZE
#endif
#ifndef FE_WIDGET_LIST_INIT_SIZE
#define FE_WIDGET_LIST_INIT_SIZE \
	DEFAULT_FE_WIDGET_LIST_INIT_SIZE
#endif
#ifndef FE_WIDGET_LIST_LOAD_FACTOR
#define FE_WIDGET_LIST_LOAD_FACTOR \
	DEFAULT_FE_WIDGET_LIST_LOAD_FACTOR
#endif
#ifndef FE_WIDGET_LIST_GROWTH_FACTOR
#define FE_WIDGET_LIST_GROWTH_FACTOR \
	DEFAULT_FE_WIDGET_LIST_GROWTH_FACTOR
#endif
#ifndef FE_LAYOUT_TYPES_INIT_SIZE
#define FE_LAYOUT_TYPES_INIT_SIZE \
	DEFAULT_FE_LAYOUT_TYPES_INIT_SIZE
#endif
#ifndef FE_LAYOUT_TYPES_LOAD_FACTOR
#define FE_LAYOUT_TYPES_LOAD_FACTOR \
	DEFAULT_FE_LAYOUT_TYPES_LOAD_FACTOR
#endif
#ifndef FE_LAYOUT_TYPES_GROWTH_FACTOR
#define FE_LAYOUT_TYPES_GROWTH_FACTOR \
	DEFAULT_FE_LAYOUT_TYPES_GROWTH_FACTOR
#endif
#ifndef FE_LAYOUTS_INIT_SIZE
#define FE_LAYOUTS_INIT_SIZE \
	DEFAULT_FE_LAYOUTS_INIT_SIZE
#endif
#ifndef FE_LAYOUTS_LOAD_FACTOR
#define FE_LAYOUTS_LOAD_FACTOR \
	DEFAULT_FE_LAYOUTS_LOAD_FACTOR
#endif
#ifndef FE_LAYOUTS_GROWTH_FACTOR
#define FE_LAYOUTS_GROWTH_FACTOR \
	DEFAULT_FE_LAYOUTS_GROWTH_FACTOR
#endif
#ifndef FE_COLOURMAP_SIZE
#define FE_COLOURMAP_SIZE \
	DEFAULT_FE_COLOURMAP_SIZE
#endif
#ifndef FE_FORMAT_MAX_STYLES
#define FE_FORMAT_MAX_STYLES \
	DEFAULT_FE_FORMAT_MAX_STYLES
#endif
#ifndef FE_NET_ERR_RETRYING
#define FE_NET_ERR_RETRYING \
	DEFAULT_FE_NET_ERR_RETRYING
#endif
#ifndef FE_NET_ERR_FAILED_TO_CONNECT
#define FE_NET_ERR_FAILED_TO_CONNECT \
	DEFAULT_FE_NET_ERR_FAILED_TO_CONNECT
#endif
#ifndef FE_THEME_BRACKET
#define FE_THEME_BRACKET \
	DEFAULT_FE_THEME_BRACKET
#endif
#ifndef FE_THEME_CHANNEL
#define FE_THEME_CHANNEL \
	DEFAULT_FE_THEME_CHANNEL
#endif
#ifndef FE_THEME_DEFAULT
#define FE_THEME_DEFAULT \
	DEFAULT_FE_THEME_DEFAULT
#endif
#ifndef FE_THEME_ERROR
#define FE_THEME_ERROR \
	DEFAULT_FE_THEME_ERROR
#endif
#ifndef FE_THEME_MESSAGE
#define FE_THEME_MESSAGE \
	DEFAULT_FE_THEME_MESSAGE
#endif
#ifndef FE_THEME_NICK
#define FE_THEME_NICK \
	DEFAULT_FE_THEME_NICK
#endif
#ifndef FE_THEME_SERVER
#define FE_THEME_SERVER \
	DEFAULT_FE_THEME_SERVER
#endif
#ifndef FE_THEME_STATUS
#define FE_THEME_STATUS \
	DEFAULT_FE_THEME_STATUS
#endif
#ifndef FE_THEME_STATUSBAR
#define FE_THEME_STATUSBAR \
	DEFAULT_FE_THEME_STATUSBAR
#endif
#ifndef FE_THEME_TIMESTAMP
#define FE_THEME_TIMESTAMP \
	DEFAULT_FE_THEME_TIMESTAMP
#endif
#ifndef FE_THEME_TOPIC
#define FE_THEME_TOPIC \
	DEFAULT_FE_THEME_TOPIC
#endif
#ifndef FE_COMMON_ATTRIBS
#define FE_COMMON_ATTRIBS() \
	DEFAULT_FE_COMMON_ATTRIBS()
#endif
#ifndef FE_COMMON_COMMANDS
#define FE_COMMON_COMMANDS() \
	DEFAULT_FE_COMMON_COMMANDS()
#endif
#ifndef FE_COMMON_FORMATS
#define FE_COMMON_FORMATS() \
	DEFAULT_FE_COMMON_FORMATS()
#endif
#ifndef FE_BINDINGS
#define FE_BINDINGS() \
	DEFAULT_FE_BINDINGS()
#endif
#ifndef FE_NET_ERROR_JOINPOINT
#define FE_NET_ERROR_JOINPOINT(...) \
	DEFAULT_FE_NET_ERROR_JOINPOINT(__VA_ARGS__)
#endif
/*** END OF DEFAULT ASSIGNMENTS ***/


#endif


