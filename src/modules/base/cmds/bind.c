/*
 * Command Name:	bind.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; variable ; frontend ; string ; modbase
 * Description:		Bind Key Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>
#include <stutter/modules/base.h>

int base_cmd_bind(char *env, char *args)
{
	void *value;
	void *window;
	char *key, *name;
	struct type_s *type;
	char *context = NULL;
	char buffer[SMALL_STRING_SIZE];

	trim_whitespace_m(args);
	if (*args == '-')
		split_string_m(&args[1], context, args, ' ');
	get_param_m(args, key, ' ');
	get_param_m(args, name, ' ');

	if (!(value = index_variable(NULL, PATH_VARIABLE_NAME, name, &type)) && !(value = find_variable(NULL, name, &type))) {
		BASE_ERROR_JOINPOINT(BASE_ERR_VARIABLE_NOT_FOUND, name);
		return(-1);
	}
	else if ((util_convert_key(key, buffer, SMALL_STRING_SIZE) < 0) || bind_key(context, buffer, value, type, args)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_BINDING_FAILED, key);
		return(-1);
	}
	else if ((window = fe_current_widget("text", NULL)) || (window = fe_first_widget("text", NULL))) {
		if (snprintf(buffer, SMALL_STRING_SIZE, "Key %s bound to %s %s", key, name, args) >= 0)
			fe_print(window, buffer);
	}
	return(0);
}


