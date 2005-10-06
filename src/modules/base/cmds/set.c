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

int base_cmd_set(char *env, char *str)
{
	char *name;
	void *window;
	string_t value;
	char *ns_name = NULL;

	get_param_m(str, name, ' ');
	if (!(window = fe_next_widget()) && !(window = fe_first_widget()))
		return(-1);
	if (strchr(name, ':')) {
		get_param_m(name, ns_name, ':');
	}
	if (add_variable(find_type("string"), ns_name, name, create_string(str)))
		fe_print(window, create_string("Error setting variable."));
	else
		fe_print(window, create_string("Variable: %s:%s <= %s", ns_name ? ns_name : "", name, str));
	return(0);
}


