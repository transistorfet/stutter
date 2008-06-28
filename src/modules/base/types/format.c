/*
 * Type Name:		format.c
 * Description:		Format String Type
 */

#include <stdlib.h>

#include <stutter/utils.h>
#include <stutter/object.h>
#include <stutter/variable.h>
#include <stutter/modules/base/base.h>

struct variable_type_s base_format_type = { {
	OBJECT_TYPE_S(&base_string_type),
	"format",
	sizeof(struct base_string),
	NULL,
	(object_init_t) base_string_init,
	(object_release_t) base_string_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) base_format_stringify,
	(variable_evaluate_t) NULL
};

int base_format_stringify(struct base_string *var, char *buffer, int max)
{
	return(util_expand_str(var->str, buffer, max));
}

