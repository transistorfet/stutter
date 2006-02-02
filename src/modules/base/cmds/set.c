/*
 * Command Name:	set.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; variable ; frontend ; string ; modbase
 * Description:		Set Variable Command
 */

#include <string.h>

#include CONFIG_H
#include <type.h>
#include <utils.h>
#include <variable.h>
#include <frontend.h>
#include <lib/macros.h>
#include <lib/string.h>
#include <modules/base.h>

int base_cmd_set(char *env, char *args)
{
	int j = 0;
	void *window;
	char *ns, *name;
	struct type_s *type;
	struct variable_s *var;
	char *type_name = BASE_DEFAULT_SET_TYPE;


	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);

	trim_whitespace_m(args);
	if (*args == '-')
		split_string_m(&args[1], type_name, args, ' ');
	get_param_m(args, name, ' ');
	get_prefix_m(name, ns, ':');

	trim_whitespace_m(args);
	if (!(type = find_type(type_name)) || !type->create)
		fe_print(window, create_string("Error: Type %s not found.", type_name));
	else if (!(var = add_variable(type, ns, name, type->create("%s", args))))
		fe_print(window, create_string("Error setting variable."));
	else
		fe_print(window, create_string("Variable: %s:%s <= %s", var->ns->name, name, args));
	return(0);
}


