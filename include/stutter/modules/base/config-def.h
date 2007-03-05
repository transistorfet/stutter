/*
 * Header Name:	config-def.h
 * Description:	Default Base Module Configuration Header
 */

#ifndef _MODULES_BASE_CONFIG_DEF_H
#define _MODULES_BASE_CONFIG_DEF_H

#include <stutter/config-macros.h>

#define DEFAULT_BASE_NAMESPACE			"base"
#define DEFAULT_BASE_DEFAULT_PATH		";base"

#define DEFAULT_BASE_TIME_FORMAT		"%H:%M"
#define DEFAULT_BASE_DATE_FORMAT		"%d-%b"
#define DEFAULT_BASE_TIMESTAMP_FORMAT		"%d-%b:%H:%M"

#define DEFAULT_BASE_COMMAND_PREFIX		"/"
#define DEFAULT_BASE_NULL_COMMAND		""

#define DEFAULT_BASE_FMT_ALIAS			"Alias: %s <= %s"
#define DEFAULT_BASE_FMT_BIND			"Key %s bound to %s %s"
#define DEFAULT_BASE_FMT_REDIRECT_DISCONNECTED	"Signal %s has been disconnected"
#define DEFAULT_BASE_FMT_REDIRECT_CURRENT	"Signal %s is directed to the current window"
#define DEFAULT_BASE_FMT_REDIRECT_THIS		"Signal %s is directed to this window"
#define DEFAULT_BASE_FMT_REMOVE			"Variable %s removed"
#define DEFAULT_BASE_FMT_SET			"Variable: %s <= %s"
#define DEFAULT_BASE_FMT_UNBIND			"Key %s is not longer bound"

#define DEFAULT_BASE_ERR_UNKNOWN_COMMAND	"\022error\022Error: Unknown Command %s"
#define DEFAULT_BASE_ERR_UNABLE_TO_OPEN_FILE	"\022error\022Error: Unable to open the file %s"
#define DEFAULT_BASE_ERR_VARIABLE_NOT_FOUND	"\022error\022Error: %s variable not found"
#define DEFAULT_BASE_ERR_TYPE_NOT_FOUND		"\022error\022Error: %s type not found"
#define DEFAULT_BASE_ERR_STRINGIFY_FAILED	"\022error\022Error stringifying variable %s"
#define DEFAULT_BASE_ERR_EVALUATE_FAILED	"\022error\022Error evaluating variable %s"
#define DEFAULT_BASE_ERR_ALIAS_FAILED		"\022error\022Error creating alias %s"
#define DEFAULT_BASE_ERR_BINDING_FAILED		"\022error\022Error binding key %s"
#define DEFAULT_BASE_ERR_REDIRECT_FAILED	"\022error\022Error: Invalid target location %s"
#define DEFAULT_BASE_ERR_REMOVE_FAILED		"\022error\022Error removing variable %s"
#define DEFAULT_BASE_ERR_SET_FAILED		"\022error\022Error setting variable %s"
#define DEFAULT_BASE_ERR_UNBINDING_FAILED	"\022error\022Error unbinding key %s"

#define DEFAULT_BASE_TYPES()		\
	ADD_TYPE(base_load_command)	\
	ADD_TYPE(base_load_format)	\
	ADD_TYPE(base_load_status)	\
	ADD_TYPE(base_load_string)	\
	ADD_TYPE(base_load_table)	\
	ADD_TYPE(base_load_time)	\
	ADD_TYPE(base_load_wildcard)

#define DEFAULT_BASE_BINDINGS()			\
	BIND_KEY("\n", "base.parse", "")	\
	BIND_KEY("\x03", "base.quit", "")	\
	BIND_KEY("\x18", "base.next", "")	\
	BIND_KEY("\x15", "base.clearline", "")	\
	BIND_KEY("\x11", "base.previous", "")	\
	BIND_KEY("\x10", "base.scroll", "1")	\
	BIND_KEY("\x19", "base.scroll", "-1")

#define DEFAULT_BASE_HANDLERS()						\
	ADD_HANDLER(NULL, "error_general", 10, base_sig_print, NULL)	\
	ADD_HANDLER(NULL, "base.error", 10, base_sig_print, NULL)

#define DEFAULT_BASE_COMMANDS()				\
	ADD_COMMAND("alias", base_cmd_alias)		\
	ADD_COMMAND("bind", base_cmd_bind)		\
	ADD_COMMAND("clear", base_cmd_clear)		\
	ADD_COMMAND("clearline", base_cmd_clearline)	\
	ADD_COMMAND("close", base_cmd_close)		\
	ADD_COMMAND("echo", base_cmd_echo)		\
	ADD_COMMAND("exec", base_cmd_exec)		\
	ADD_COMMAND("expand", base_cmd_expand)		\
	ADD_COMMAND("get", base_cmd_get)		\
	ADD_COMMAND("next", base_cmd_next)		\
	ADD_COMMAND("parse", base_cmd_parse)		\
	ADD_COMMAND("previous", base_cmd_previous)	\
	ADD_COMMAND("quit", base_cmd_quit)		\
	ADD_COMMAND("redirect", base_cmd_redirect)	\
	ADD_COMMAND("remove", base_cmd_remove)		\
	ADD_COMMAND("scroll", base_cmd_scroll)		\
	ADD_COMMAND("select", base_cmd_select)		\
	ADD_COMMAND("set", base_cmd_set)		\
	ADD_COMMAND("source", base_cmd_source)		\
	ADD_COMMAND("timer", base_cmd_timer)		\
	ADD_COMMAND("unbind", base_cmd_unbind)

/*** Joinpoints ***/

#define DEFAULT_BASE_INIT_JOINPOINT(table)	\
	DO_NOTHING();

#define DEFAULT_BASE_RELEASE_JOINPOINT(table)	\
	DO_NOTHING();

#define DEFAULT_BASE_COMMAND_RESPONSE_JOINPOINT(...)	\
	BASE_OUTPUT_JOINPOINT(__VA_ARGS__)

#define DEFAULT_BASE_ERROR_JOINPOINT(...)	\
	ERROR_JOINPOINT("base.error", __VA_ARGS__)

#define DEFAULT_BASE_OUTPUT_JOINPOINT(...)	\
	OUTPUT_JOINPOINT("base.output", __VA_ARGS__)

/*** START OF DEFAULT ASSIGNMENTS ***/
#ifndef BASE_NAMESPACE
#define BASE_NAMESPACE \
	DEFAULT_BASE_NAMESPACE
#endif
#ifndef BASE_DEFAULT_PATH
#define BASE_DEFAULT_PATH \
	DEFAULT_BASE_DEFAULT_PATH
#endif
#ifndef BASE_TIME_FORMAT
#define BASE_TIME_FORMAT \
	DEFAULT_BASE_TIME_FORMAT
#endif
#ifndef BASE_DATE_FORMAT
#define BASE_DATE_FORMAT \
	DEFAULT_BASE_DATE_FORMAT
#endif
#ifndef BASE_TIMESTAMP_FORMAT
#define BASE_TIMESTAMP_FORMAT \
	DEFAULT_BASE_TIMESTAMP_FORMAT
#endif
#ifndef BASE_COMMAND_PREFIX
#define BASE_COMMAND_PREFIX \
	DEFAULT_BASE_COMMAND_PREFIX
#endif
#ifndef BASE_NULL_COMMAND
#define BASE_NULL_COMMAND \
	DEFAULT_BASE_NULL_COMMAND
#endif
#ifndef BASE_FMT_ALIAS
#define BASE_FMT_ALIAS \
	DEFAULT_BASE_FMT_ALIAS
#endif
#ifndef BASE_FMT_BIND
#define BASE_FMT_BIND \
	DEFAULT_BASE_FMT_BIND
#endif
#ifndef BASE_FMT_REDIRECT_DISCONNECTED
#define BASE_FMT_REDIRECT_DISCONNECTED \
	DEFAULT_BASE_FMT_REDIRECT_DISCONNECTED
#endif
#ifndef BASE_FMT_REDIRECT_CURRENT
#define BASE_FMT_REDIRECT_CURRENT \
	DEFAULT_BASE_FMT_REDIRECT_CURRENT
#endif
#ifndef BASE_FMT_REDIRECT_THIS
#define BASE_FMT_REDIRECT_THIS \
	DEFAULT_BASE_FMT_REDIRECT_THIS
#endif
#ifndef BASE_FMT_REMOVE
#define BASE_FMT_REMOVE \
	DEFAULT_BASE_FMT_REMOVE
#endif
#ifndef BASE_FMT_SET
#define BASE_FMT_SET \
	DEFAULT_BASE_FMT_SET
#endif
#ifndef BASE_FMT_UNBIND
#define BASE_FMT_UNBIND \
	DEFAULT_BASE_FMT_UNBIND
#endif
#ifndef BASE_ERR_UNKNOWN_COMMAND
#define BASE_ERR_UNKNOWN_COMMAND \
	DEFAULT_BASE_ERR_UNKNOWN_COMMAND
#endif
#ifndef BASE_ERR_UNABLE_TO_OPEN_FILE
#define BASE_ERR_UNABLE_TO_OPEN_FILE \
	DEFAULT_BASE_ERR_UNABLE_TO_OPEN_FILE
#endif
#ifndef BASE_ERR_VARIABLE_NOT_FOUND
#define BASE_ERR_VARIABLE_NOT_FOUND \
	DEFAULT_BASE_ERR_VARIABLE_NOT_FOUND
#endif
#ifndef BASE_ERR_TYPE_NOT_FOUND
#define BASE_ERR_TYPE_NOT_FOUND \
	DEFAULT_BASE_ERR_TYPE_NOT_FOUND
#endif
#ifndef BASE_ERR_STRINGIFY_FAILED
#define BASE_ERR_STRINGIFY_FAILED \
	DEFAULT_BASE_ERR_STRINGIFY_FAILED
#endif
#ifndef BASE_ERR_EVALUATE_FAILED
#define BASE_ERR_EVALUATE_FAILED \
	DEFAULT_BASE_ERR_EVALUATE_FAILED
#endif
#ifndef BASE_ERR_ALIAS_FAILED
#define BASE_ERR_ALIAS_FAILED \
	DEFAULT_BASE_ERR_ALIAS_FAILED
#endif
#ifndef BASE_ERR_BINDING_FAILED
#define BASE_ERR_BINDING_FAILED \
	DEFAULT_BASE_ERR_BINDING_FAILED
#endif
#ifndef BASE_ERR_REDIRECT_FAILED
#define BASE_ERR_REDIRECT_FAILED \
	DEFAULT_BASE_ERR_REDIRECT_FAILED
#endif
#ifndef BASE_ERR_REMOVE_FAILED
#define BASE_ERR_REMOVE_FAILED \
	DEFAULT_BASE_ERR_REMOVE_FAILED
#endif
#ifndef BASE_ERR_SET_FAILED
#define BASE_ERR_SET_FAILED \
	DEFAULT_BASE_ERR_SET_FAILED
#endif
#ifndef BASE_ERR_UNBINDING_FAILED
#define BASE_ERR_UNBINDING_FAILED \
	DEFAULT_BASE_ERR_UNBINDING_FAILED
#endif
#ifndef BASE_TYPES
#define BASE_TYPES() \
	DEFAULT_BASE_TYPES()
#endif
#ifndef BASE_BINDINGS
#define BASE_BINDINGS() \
	DEFAULT_BASE_BINDINGS()
#endif
#ifndef BASE_HANDLERS
#define BASE_HANDLERS() \
	DEFAULT_BASE_HANDLERS()
#endif
#ifndef BASE_COMMANDS
#define BASE_COMMANDS() \
	DEFAULT_BASE_COMMANDS()
#endif
#ifndef BASE_INIT_JOINPOINT
#define BASE_INIT_JOINPOINT(table) \
	DEFAULT_BASE_INIT_JOINPOINT(table)
#endif
#ifndef BASE_RELEASE_JOINPOINT
#define BASE_RELEASE_JOINPOINT(table) \
	DEFAULT_BASE_RELEASE_JOINPOINT(table)
#endif
#ifndef BASE_COMMAND_RESPONSE_JOINPOINT
#define BASE_COMMAND_RESPONSE_JOINPOINT(...) \
	DEFAULT_BASE_COMMAND_RESPONSE_JOINPOINT(__VA_ARGS__)
#endif
#ifndef BASE_ERROR_JOINPOINT
#define BASE_ERROR_JOINPOINT(...) \
	DEFAULT_BASE_ERROR_JOINPOINT(__VA_ARGS__)
#endif
#ifndef BASE_OUTPUT_JOINPOINT
#define BASE_OUTPUT_JOINPOINT(...) \
	DEFAULT_BASE_OUTPUT_JOINPOINT(__VA_ARGS__)
#endif
/*** END OF DEFAULT ASSIGNMENTS ***/


#endif


