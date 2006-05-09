/*
 * Command Name:	get.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Get Variable Command
 */

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
	int i;
	char *name;
	void *window;
	struct variable_s *var;
	char buffer[STRING_SIZE];

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	get_param_m(args, name, ' ');
	if (!(var = find_variable(NULL, name)) || !var->type->stringify) {
		fe_print(window, "Variable is undefined.");
		return(-1);
	}
	else {
		i = snprintf(buffer, STRING_SIZE, "Variable: %s = ", name);
		if (var->type->stringify(var->value, &buffer[i], STRING_SIZE - i) < 0)
			snprintf(buffer, STRING_SIZE, "Error stringifying variable, %s.", name);
		fe_print(window, buffer);
	}
	return(0);
}


