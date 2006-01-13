/*
 * Command Name:	remove.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Remove Variable Command
 */

#include <string.h>

#include <type.h>
#include <variable.h>
#include <frontend.h>
#include <lib/macros.h>
#include <lib/string.h>
#include <modules/base.h>

int base_cmd_remove(char *env, char *args)
{
	void *window;
	char *ns, *name;
	struct variable_s *var;

	get_param_m(args, name, ' ');
	if (!(window = fe_next_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	get_prefix_m(name, ns, ':');
	if (!remove_variable(find_type("string"), ns, name))
		fe_print(window, create_string("Variable %s:%s removed.", ns ? ns : "", name));
	else
		fe_print(window, create_string("Error removing variable."));
	return(0);
}


