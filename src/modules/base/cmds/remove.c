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
	struct variable_s *var;
	char buffer[STRING_SIZE];

	get_param_m(args, name, ' ');
	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	if (!remove_variable(NULL, find_type("string"), name)) {
		if (snprintf(buffer, STRING_SIZE, "Variable %s removed.", name) >= 0)
			fe_print(window, buffer);
		return(0);
	}
	else {
		fe_print(window, "Error removing variable.");
		return(-1);
	}
}


