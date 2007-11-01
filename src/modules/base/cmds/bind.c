/*
 * Command Name:	bind.c
 * Description:		Bind Key Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/key.h>
#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_bind(char *env, char *args)
{
	int pos = 0;
	void *value;
	char *key, *name;
	struct type_s *type;
	char *context = NULL;
	int buffer[SMALL_STRING_SIZE];

	TRIM_WHITESPACE(args);
	if (*args == '-') {
		pos = 1;
		context = util_get_arg(&args[1], &pos);
	}
	key = util_get_arg(&args[pos], &pos);
	name = util_get_arg(&args[pos], &pos);

	if (!(value = index_variable(NULL, PATH_VARIABLE_NAME, name, &type)) && !(value = find_variable(NULL, name, &type))) {
		BASE_ERROR_JOINPOINT(BASE_ERR_VARIABLE_NOT_FOUND, name);
		return(-1);
	}
	else if ((util_convert_key(key, buffer, SMALL_STRING_SIZE) < 0) || bind_key(context, buffer, value, type, &args[pos])) {
		BASE_ERROR_JOINPOINT(BASE_ERR_BINDING_FAILED, key);
		return(-1);
	}
	BASE_COMMAND_RESPONSE_JOINPOINT(BASE_FMT_BIND, key, name, &args[pos])
	return(0);
}


