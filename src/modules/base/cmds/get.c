/*
 * Command Name:	get.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Get Variable Command
 */

#include <string.h>

#include <type.h>
#include <macros.h>
#include <variable.h>
#include <frontend.h>
#include <nit/string.h>
#include <modules/base.h>

int base_cmd_get(char *env, char *args)
{
	char *name;
	void *window;
	char *ns_name = NULL;
	struct variable_s *var;

	get_param_m(args, name, ' ');
	if (!(window = fe_next_widget()) && !(window = fe_first_widget()))
		return(-1);
	if (strchr(name, ':')) {
		get_param_m(name, ns_name, ':');
	}
	if (var = find_variable(find_type("string"), ns_name, name))
		fe_print(window, create_string("Variable: %s:%s = %s", ns_name ? ns_name : "", name, var->value));
	else
		fe_print(window, create_string("Variable is undefined."));
	return(0);
}


