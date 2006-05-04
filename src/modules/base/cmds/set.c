/*
 * Command Name:	set.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Set Variable Command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>
#include <stutter/modules/base.h>

int base_cmd_set(char *env, char *args)
{
	int j = 0;
	char *name;
	void *window;
	struct type_s *type;
	struct variable_s *var;
	char *type_name = BASE_DEFAULT_SET_TYPE;


	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);

	trim_whitespace_m(args);
	if (*args == '-')
		split_string_m(&args[1], type_name, args, ' ');
	get_param_m(args, name, ' ');

	trim_whitespace_m(args);
	if (!(type = find_type(type_name)) || !type->create)
		fe_print(window, create_string("Error: Type %s not found.", type_name));
	else if (!(var = add_variable(NULL, type, name, 0, "%s", args)))
		fe_print(window, create_string("Error setting variable."));
	else
		fe_print(window, create_string("Variable: %s <= %s", name, args));
	return(0);
}


