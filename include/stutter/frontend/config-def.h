/*
 * Header Name:	config-def.h
 * Description:	Default Frontend Configuration Header
 */

#ifndef _FRONTEND_CONFIG_DEF_H
#define _FRONTEND_CONFIG_DEF_H

#include <stutter/config-macros.h>

#define DEFAULT_FE_STATUSBAR			"$&fe.statusbar"
#define DEFAULT_FE_STATUSBAR_DEFAULT		"${fe.inverse} $time $irc.current_nick on $irc.current_channel"
#define DEFAULT_FE_STATUS_BAR_HEIGHT		1
#define DEFAULT_FE_WINDOW_LOG_SIZE		1000
#define DEFAULT_FE_WINDOW_WRAP_STRING		"        "
#define DEFAULT_FE_INPUT_BUFFER_SIZE		1024
#define DEFAULT_FE_INPUT_HISTORY_SIZE		200

#define DEFAULT_FE_NET_ERR_RETRYING		"Error: Failed to connect to %s, Retrying..."
#define DEFAULT_FE_NET_ERR_FAILED_TO_CONNECT	"Error: Failed to connect to %s"

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
#ifndef FE_WINDOW_LOG_SIZE
#define FE_WINDOW_LOG_SIZE \
	DEFAULT_FE_WINDOW_LOG_SIZE
#endif
#ifndef FE_WINDOW_WRAP_STRING
#define FE_WINDOW_WRAP_STRING \
	DEFAULT_FE_WINDOW_WRAP_STRING
#endif
#ifndef FE_INPUT_BUFFER_SIZE
#define FE_INPUT_BUFFER_SIZE \
	DEFAULT_FE_INPUT_BUFFER_SIZE
#endif
#ifndef FE_INPUT_HISTORY_SIZE
#define FE_INPUT_HISTORY_SIZE \
	DEFAULT_FE_INPUT_HISTORY_SIZE
#endif
#ifndef FE_NET_ERR_RETRYING
#define FE_NET_ERR_RETRYING \
	DEFAULT_FE_NET_ERR_RETRYING
#endif
#ifndef FE_NET_ERR_FAILED_TO_CONNECT
#define FE_NET_ERR_FAILED_TO_CONNECT \
	DEFAULT_FE_NET_ERR_FAILED_TO_CONNECT
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


