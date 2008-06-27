/*
 * Type Name:		status.c
 * Description:		Status (Stringify Callback) Type
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <stutter/memory.h>
#include <stutter/string.h>
#include <stutter/object.h>
#include <stutter/modules/base.h>

struct variable_type_s base_status_type = { {
	OBJECT_TYPE_S(&variable_type),
	"status",
	sizeof(struct base_status_s),
	NULL,
	(object_init_t) base_status_init,
	(object_release_t) base_status_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) base_status_stringify,
	(variable_evaluate_t) NULL
};

int base_status_init(struct base_status_s *var, const char *params, va_list va)
{
	if (params[0] != 'f')
		return(-1);
	var->func = va_arg(va, stringify_t);
	if (params[1] != '\0')
		var->ptr = va_arg(va, void *);
	return(0);
}

void base_status_release(struct base_status_s *var)
{
	// TODO free the ptr?
}

int base_status_stringify(struct base_status_s *var, char *buffer, int max)
{
	return(var->func(var->ptr, buffer, max));
}




