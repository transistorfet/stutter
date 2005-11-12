/*
 * Command Name:	set.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Set Variable Command
 */

#include <string.h>

#include <type.h>
#include <macros.h>
#include <variable.h>
#include <frontend.h>
#include <nit/string.h>
#include <modules/base.h>

int base_cmd_set(char *env, char *args)
{
	void *window;
	string_t value;
	char *ns, *name;

	get_param_m(args, name, ' ');
	if (!(window = fe_next_widget()) && !(window = fe_first_widget()))
		return(-1);
	get_prefix_m(name, ns, ':');
	if (add_variable(find_type("string"), ns, name, create_string(args)))
		fe_print(window, create_string("Variable: %s:%s <= %s", ns ? ns : "", name, args));
	else
		fe_print(window, create_string("Error setting variable."));
	return(0);
}


