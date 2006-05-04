/*
 * Command Name:	remove.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; string ; modbase
 * Description:		Remove Variable Command
 */

#include <string.h>

#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>
#include <stutter/modules/base.h>

int base_cmd_remove(char *env, char *args)
{
	char *name;
	void *window;
	struct variable_s *var;

	get_param_m(args, name, ' ');
	if (!(window = fe_current_widget("window", NULL)) && !(window = fe_first_widget("window", NULL)))
		return(-1);
	if (!remove_variable(NULL, find_type("string"), name))
		fe_print(window, create_string("Variable %s removed.", name));
	else
		fe_print(window, create_string("Error removing variable."));
	return(0);
}


