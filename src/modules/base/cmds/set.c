/*
 * Command Name:	set.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Set Variable Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_set(char *env, char *args)
{
	int j = 0;
	char *name;
	void *window;
	char *type_name = NULL;
	char buffer[STRING_SIZE];
	struct type_s *type = NULL;

	trim_whitespace_m(args);
	if (*args == '-')
		split_string_m(&args[1], type_name, args, ' ');
	get_param_m(args, name, ' ');

	trim_whitespace_m(args);
	if (type_name && (!(type = find_type(type_name)) || !type->create)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_TYPE_NOT_FOUND, type_name)
		return(-1);
	}
	else if (!(add_variable(NULL, type, name, 0, "string", args))) {
		BASE_ERROR_JOINPOINT(BASE_ERR_SET_FAILED, name)
		return(-1);
	}
	else if ((window = fe_current_widget("text", NULL)) || (window = fe_first_widget("text", NULL))) {
		if (snprintf(buffer, STRING_SIZE, "Variable: %s <= %s", name, args) >= 0)
			fe_print(window, buffer);
	}
	return(0);
}


