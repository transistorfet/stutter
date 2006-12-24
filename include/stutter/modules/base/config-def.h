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
#define DEFAULT_BASE_DEFAULT_SET_TYPE		"string"

#define DEFAULT_BASE_COMMAND_PREFIX		"/"
#define DEFAULT_BASE_NULL_COMMAND		""

#define DEFAULT_BASE_ERR_UNKNOWN_COMMAND	"Error: Unknown Command %s"
#define DEFAULT_BASE_ERR_UNABLE_TO_OPEN_FILE	"Error: Unable to open the file %s"
#define DEFAULT_BASE_ERR_VARIABLE_NOT_FOUND	"Error: %s variable not found"
#define DEFAULT_BASE_ERR_TYPE_NOT_FOUND		"Error: %s type not found"
#define DEFAULT_BASE_ERR_STRINGIFY_FAILED	"Error stringifying variable %s"
#define DEFAULT_BASE_ERR_EVALUATE_FAILED	"Error evaluating variable %s"
#define DEFAULT_BASE_ERR_ALIAS_FAILED		"Error creating alias %s"
#define DEFAULT_BASE_ERR_BINDING_FAILED		"Error binding key %s"
#define DEFAULT_BASE_ERR_REMOVE_FAILED		"Error removing variable %s"
#define DEFAULT_BASE_ERR_SET_FAILED		"Error setting variable %s"
#define DEFAULT_BASE_ERR_UNBINDING_FAILED	"Error unbinding key %s"

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
	BIND_KEY("\x18", "base.next", "")	\
	BIND_KEY("\x15", "base.clearline", "")	\
	BIND_KEY("\x11", "base.previous", "")	\
	BIND_KEY("\x10", "base.scroll", "1")	\
	BIND_KEY("\x19", "base.scroll", "-1")

#define DEFAULT_BASE_HANDLERS()						\
	ADD_HANDLER("error_general", NULL, 10, base_sig_print, NULL)	\
	ADD_HANDLER("base.error", NULL, 10, base_sig_print, NULL)

#define DEFAULT_BASE_COMMANDS()				\
	ADD_COMMAND("alias", base_cmd_alias)		\
	ADD_COMMAND("bind", base_cmd_bind)		\
	ADD_COMMAND("clear", base_cmd_clear)		\
	ADD_COMMAND("clearline", base_cmd_clearline)	\
	ADD_COMMAND("echo", base_cmd_echo)		\
	ADD_COMMAND("exec", base_cmd_exec)		\
	ADD_COMMAND("get", base_cmd_get)		\
	ADD_COMMAND("next", base_cmd_next)		\
	ADD_COMMAND("parse", base_cmd_parse)		\
	ADD_COMMAND("previous", base_cmd_previous)	\
	ADD_COMMAND("quit", base_cmd_quit)		\
	ADD_COMMAND("redirect", base_cmd_redirect)	\
	ADD_COMMAND("remove", base_cmd_remove)		\
	ADD_COMMAND("scroll", base_cmd_scroll)		\
	ADD_COMMAND("set", base_cmd_set)		\
	ADD_COMMAND("source", base_cmd_source)		\
	ADD_COMMAND("unbind", base_cmd_unbind)

#define DEFAULT_BASE_INIT_JOINPOINT(table)	\
	DO_NOTHING();

#define DEFAULT_BASE_RELEASE_JOINPOINT(table)	\
	DO_NOTHING();

#define DEFAULT_BASE_ERROR_JOINPOINT(...)	\
	ERROR_JOINPOINT("base.error", __VA_ARGS__)

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
#ifndef BASE_DEFAULT_SET_TYPE
#define BASE_DEFAULT_SET_TYPE \
	DEFAULT_BASE_DEFAULT_SET_TYPE
#endif
#ifndef BASE_COMMAND_PREFIX
#define BASE_COMMAND_PREFIX \
	DEFAULT_BASE_COMMAND_PREFIX
#endif
#ifndef BASE_NULL_COMMAND
#define BASE_NULL_COMMAND \
	DEFAULT_BASE_NULL_COMMAND
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
#ifndef BASE_ERROR_JOINPOINT
#define BASE_ERROR_JOINPOINT(...) \
	DEFAULT_BASE_ERROR_JOINPOINT(__VA_ARGS__)
#endif
/*** END OF DEFAULT ASSIGNMENTS ***/


#endif


