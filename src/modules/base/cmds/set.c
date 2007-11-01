/*
 * Command Name:	set.c
 * Description:		Set Variable Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_set(char *env, char *args)
{
	char *name;
	int pos = 0;
	char *type_name = NULL;
	struct type_s *type = NULL;

	TRIM_WHITESPACE(args);
	if (*args == '-') {
		pos = 1;
		type_name = util_get_arg(&args[1], &pos);
	}
	name = util_get_arg(&args[pos], &pos);

	if (type_name && (!(type = find_type(type_name)) || !type->create)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_TYPE_NOT_FOUND, type_name)
		return(-1);
	}
	else if (!(add_variable(NULL, type, name, 0, "string", &args[pos]))) {
		BASE_ERROR_JOINPOINT(BASE_ERR_SET_FAILED, name)
		return(-1);
	}
	BASE_COMMAND_RESPONSE_JOINPOINT(BASE_FMT_SET, name, &args[pos])
	return(0);
}


