/*
 * Command Name:	remove.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Remove Variable Command
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

int base_cmd_remove(char *env, char *args)
{
	char *name;
	void *window;
	char buffer[SMALL_STRING_SIZE];

	get_param_m(args, name, ' ');
	if (remove_variable(NULL, NULL, name)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_REMOVE_FAILED, name)
		return(-1);
	}
	else if ((window = fe_current_widget("text", NULL)) || (window = fe_first_widget("text", NULL))) {
		if (snprintf(buffer, SMALL_STRING_SIZE, "Variable %s removed.", name) >= 0)
			fe_print(window, buffer);
	}
	return(0);
}


