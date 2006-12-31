/*
 * Command Name:	get.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Get Variable Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>
#include <stutter/modules/base.h>

int base_cmd_get(char *env, char *args)
{
	int i, ret;
	char *name;
	void *window;
	char buffer[STRING_SIZE];

	get_param_m(args, name, ' ');
	i = snprintf(buffer, STRING_SIZE, "Variable: %s = ", name);
	ret = stringify_variable(NULL, name, &buffer[i], STRING_SIZE - i);

	if (ret == VAR_ERR_NOT_FOUND) {
		BASE_ERROR_JOINPOINT(BASE_ERR_VARIABLE_NOT_FOUND, name)
		return(-1);
	}
	else if (ret < 0) {
		BASE_ERROR_JOINPOINT(BASE_ERR_STRINGIFY_FAILED, name)
		return(-1);
	}
	else if ((window = fe_current_widget("text", NULL)) || (window = fe_first_widget("text", NULL)))
		fe_print(window, buffer);
	return(0);
}


