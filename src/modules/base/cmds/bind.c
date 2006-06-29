/*
 * Command Name:	bind.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; variable ; frontend ; string ; modbase
 * Description:		Bind Key Command
 */

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
	void *window;
	char *context = NULL;
	char *key, *name;
	char buffer[STRING_SIZE];
	struct variable_s *table, *var;

	trim_whitespace_m(args);
	if (*args == '-')
		split_string_m(&args[1], context, args, ' ');
	get_param_m(args, key, ' ');
	get_param_m(args, name, ' ');

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);

	if ((table = find_variable(NULL, BASE_PARSE_PATH_VARIABLE)) && table->type->index)
		var = table->type->index(table->value, name);
	else
		var = find_variable(NULL, name);

	if (!var) {
		if (snprintf(buffer, STRING_SIZE, "Error: %s variable not found.", name) >= 0)
			fe_print(window, buffer);
		return(-1);
	}

	if ((util_convert_key(key, buffer, STRING_SIZE) < 0) || bind_key(context, buffer, var, create_string(args)))
		fe_print(window, "Error binding key");
	else {
		snprintf(buffer, STRING_SIZE, "Key %s bound to %s %s", key, name, args);
		fe_print(window, buffer);
	}
	return(0);
}


