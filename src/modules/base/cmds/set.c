/*
 * Command Name:	set.c
 * Description:		Set Variable Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/output.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/object.h>
#include <stutter/variable.h>
#include <stutter/modules/base.h>

int base_cmd_set(char *env, char *args)
{
	char *name;
	int pos = 0;
	char *type_name = NULL;
	struct object_type_s *type;

	TRIM_WHITESPACE(args);
	if (*args == '-') {
		pos = 1;
		type_name = util_get_arg(&args[1], &pos);
	}
	name = util_get_arg(&args[pos], &pos);

	if (type_name && (!(type = object_find_type(type_name, NULL)))) {
		OUTPUT_ERROR(BASE_ERR_TYPE_NOT_FOUND, type_name);
		return(-1);
	}
	else if (!(add_variable(NULL, type, name, 0, "s", &args[pos]))) {
		OUTPUT_ERROR(BASE_ERR_SET_FAILED, name);
		return(-1);
	}
	OUTPUT_STATUS(BASE_FMT_SET, name, &args[pos]);
	return(0);
}


