/*
 * Command Name:	bind.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; variable ; frontend ; string ; modbase
 * Description:		Bind Key Command
 */

#include <string.h>

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
	string_t sequence;
	char *context = NULL;
	char *key, *name;
	struct variable_s *var;

	trim_whitespace_m(args);
	if (*args == '-')
		split_string_m(&args[1], context, args, ' ');
	get_param_m(args, key, ' ');
	get_param_m(args, name, ' ');

	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	if (!(var = find_variable(NULL, name))) {
		fe_print(window, create_string("Error: %s variable not found.", name));
		return(-1);
	}

	if (!(sequence = util_convert_key(key)) || bind_key(context, sequence, var, create_string(args)))
		fe_print(window, create_string("Error binding key"));
	else
		fe_print(window, create_string("Key %s bound to %s %s", key, name, args));
	if (sequence)
		destroy_string(sequence);
	return(0);
}


