/*
 * Type Name:		time.c
 * Description:		Time Stamp Type
 */

#include <time.h>
#include <stdlib.h>

#include <stutter/string.h>
#include <stutter/object.h>
#include <stutter/variable.h>
#include <stutter/modules/base.h>

struct variable_type_s base_time_type = { {
	OBJECT_TYPE_S(&base_string_type),
	"time",
	sizeof(struct base_string_s),
	NULL,
	(object_init_t) base_string_init,
	(object_release_t) base_string_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) base_time_stringify,
	(variable_evaluate_t) NULL
};

int base_time_stringify(struct base_string_s *var, char *buffer, int max)
{
	int i;
	time_t current_time;

	current_time = time(NULL);
	if (!(i = strftime(buffer, max, var->str, localtime(&current_time))))
		return(-1);
	return(i);
}

