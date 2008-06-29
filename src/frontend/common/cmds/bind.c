/*
 * Command Name:	bind.c
 * Description:		Bind Key Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/output.h>
#include <stutter/object.h>
#include <stutter/variable.h>
#include <stutter/frontend/common/key.h>

int fe_common_cmd_bind(char *env, char *args)
{
	int pos = 0;
	char *key, *name;
	char *context = NULL;
	struct variable_s *var;
	int buffer[SMALL_STRING_SIZE];

	TRIM_WHITESPACE(args);
	if (*args == '-') {
		pos = 1;
		context = util_get_arg(&args[1], &pos);
	}
	key = util_get_arg(&args[pos], &pos);
	name = util_get_arg(&args[pos], &pos);

	if (!(var = index_variable(NULL, PATH_VARIABLE_NAME, name, NULL))
	    && !(var = find_variable(NULL, name, NULL))) {
		OUTPUT_ERROR(BASE_ERR_VARIABLE_NOT_FOUND, name);
		return(-1);
	}
	else if ((util_convert_key(key, buffer, SMALL_STRING_SIZE) < 0)
	    || fe_key_bind(context, buffer, var, &args[pos])) {
		OUTPUT_ERROR(FE_ERR_BINDING_FAILED, key);
		return(-1);
	}
	OUTPUT_STATUS(FE_FMT_BIND, key, name, &args[pos]);
	return(0);
}


