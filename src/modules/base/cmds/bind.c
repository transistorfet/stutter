/*
 * Command Name:	bind.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Bind Key Command
 */

#include <string.h>

#include <type.h>
#include <macros.h>
#include <variable.h>
#include <frontend.h>
#include <nit/string.h>
#include <modules/base.h>

int base_cmd_bind(char *env, char *args)
{
	void *window;
	char *key, *name, *ns;
	struct variable_s *var;

	get_param_m(args, key, ' ');
	get_param_m(args, name, ' ');
	get_prefix_m(name, ns, ':');

	if (!(window = fe_next_widget()) && !(window = fe_first_widget()))
		return(-1);
	if (!(var = find_variable(NULL, ns, name))) {
		fe_print(window, create_string("Error: %s:%s variable not found.", ns ? ns : "", name));
		return(-1);
	}
	// TODO convert key string to a new keystring
	if (bind_key(NULL, key, var, create_string(args)))
		fe_print(window, create_string("Error binding key"));
	else
		fe_print(window, create_string("Key %s bound to %s:%s %s", key, ns ? ns : "", name, args));
	return(0);
}


