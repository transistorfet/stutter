/*
 * Command Name:	bind.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; variable ; frontend ; string ; modbase
 * Description:		Bind Key Command
 */

#include <string.h>

#include <type.h>
#include <utils.h>
#include <variable.h>
#include <frontend.h>
#include <lib/macros.h>
#include <lib/string.h>
#include <modules/base.h>

int base_cmd_bind(char *env, char *args)
{
	void *window;
	string_t sequence;
	char *context = NULL;
	char *key, *ns, *name;
	struct variable_s *var;

	trim_whitespace_m(args);
	if (*args == '-')
		split_string_m(&args[1], context, args, ' ');
	get_param_m(args, key, ' ');
	get_param_m(args, name, ' ');
	get_prefix_m(name, ns, ':');

	if (!(window = fe_next_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	if (!(var = find_variable(NULL, ns, name))) {
		fe_print(window, create_string("Error: %s:%s variable not found.", ns ? ns : "", name));
		return(-1);
	}

	if (!(sequence = util_convert_key(key)) || bind_key(context, sequence, var, create_string(args)))
		fe_print(window, create_string("Error binding key"));
	else
		fe_print(window, create_string("Key %s bound to %s:%s %s", key, ns ? ns : "", name, args));
	if (sequence)
		destroy_string(sequence);
	return(0);
}


