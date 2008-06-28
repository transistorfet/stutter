/*
 * Type Name:		command.c
 * Description:		Command Type
 */

#include <string.h>
#include <stdarg.h>

#include <stutter/memory.h>
#include <stutter/globals.h>
#include <stutter/object.h>
#include <stutter/variable.h>
#include <stutter/modules/base/base.h>

struct variable_type_s base_command_type = { {
	OBJECT_TYPE_S(&variable_type),
	"command",
	sizeof(struct base_command),
	NULL,
	(object_init_t) base_command_init,
	(object_release_t) base_command_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) NULL,
	(variable_evaluate_t) base_command_evaluate
};

int base_command_init(struct base_command *var, const char *params, va_list va)
{
// TODO should you have a bitflag or something for whether or not you should free (and how) the ptr?
//	if (var->ptr)
//		destroy_object(var->ptr);
	if (params[0] != 'f')
		return(-1);
	// TODO the param checking isn't complete here
	var->func = va_arg(va, callback_t);
	var->ptr = va_arg(va, void *);
	return(0);
}

void base_command_release(struct base_command *var)
{
	// TODO free object?
	variable_release(VARIABLE_S(var));
}

int base_command_evaluate(struct base_command *var, void *ptr)
{
	if (var->func)
		(var->func)(var->ptr, ptr);
	// TODO this is here for some reason but we should find out why and try to get rid of it
	return(-1);
}

