/*
 * Command Name:	get.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Get Variable Command
 */

#include <string.h>

#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>
#include <stutter/modules/base.h>

int base_cmd_get(char *env, char *args)
{
	char *name;
	void *window;
	string_t value;
	struct variable_s *var;

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	get_param_m(args, name, ' ');
	if (!(var = find_variable(NULL, name)) || !var->type->stringify)
		fe_print(window, create_string("Variable is undefined."));
	else if (!(value = var->type->stringify(var->value)))
		fe_print(window, create_string("Error stringifying variable, %s.", name));
	else {
		fe_print(window, create_string("Variable: %s = %s", name, value));
		destroy_string(value);
	}

	return(0);
}


