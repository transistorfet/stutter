/*
 * Command Name:	remove.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Remove Variable Command
 */

#include <string.h>

#include <type.h>
#include <macros.h>
#include <variable.h>
#include <frontend.h>
#include <nit/string.h>
#include <modules/base.h>

int base_cmd_remove(char *env, char *args)
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
	if (!remove_variable(find_type("string"), ns_name, name))
		fe_print(window, create_string("Variable %s:%s removed.", ns_name ? ns_name : "", name));
	else
		fe_print(window, create_string("Error removing variable."));
	return(0);
}


