/*
 * Type Name:		string.c
 * Description:		String Type
 */

#include <stdlib.h>

#include <stutter/string.h>
#include <stutter/variable.h>
#include <stutter/modules/base/base.h>

struct variable_type_s base_string_type = { {
	OBJECT_TYPE_S(&variable_type),
	"string",
	sizeof(struct base_string_s),
	NULL,
	(object_init_t) base_string_init,
	(object_release_t) base_string_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) base_string_stringify,
	(variable_evaluate_t) NULL
};

int base_string_init(struct base_string_s *var, const char *params, va_list va)
{
	char *str;

	if (var->str)
		destroy_string(var->str);
	if (params[0] != 's')
		return(-1);
	str = va_arg(va, char *);
	var->str = create_string("%s", str);
	return(0);	
}

void base_string_release(struct base_string_s *var)
{
	// TODO should we do this manually instead?
	destroy_string(var->str);
	variable_release(VARIABLE_S(var));
}

int base_string_stringify(struct base_string_s *var, char *buffer, int max)
{
	strncpy(buffer, var->str, max);
	buffer[max - 1] = '\0';
	// TODO should we do this?
	return(strlen(buffer));
}


