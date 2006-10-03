/*
 * Command Name:	alias.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; modbase
 * Description:		Create Alias Command
 */

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/modules/base.h>

int base_cmd_alias(char *env, char *args)
{
	void *window;
	char *name, *str;
	struct type_s *type;
	char buffer[STRING_SIZE];

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	trim_whitespace_m(args);
	get_param_m(args, name, ' ');

	if (!(type = find_type("command")) || !type->create) {
		if (snprintf(buffer, STRING_SIZE, "Error: Command type not found.") >= 0)
			fe_print(window, buffer);
		return(-1);
	}
	else if (!(str = create_string(args)) || !(add_variable(NULL, type, name, 0, "%r%s", base_cmd_evaluate, str))) {
		fe_print(window, "Error setting alias.");
		return(-1);
	}
	else {
		if (snprintf(buffer, STRING_SIZE, "Alias: %s <= %s", name, args) >= 0)
			fe_print(window, buffer);
	}
	return(0);
}


