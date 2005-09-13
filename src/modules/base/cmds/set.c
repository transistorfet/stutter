/*
 * Command Name:	set.c
 * Version:		0.1
 * Module Requirements:	frontend ; variable ; string ; modbase
 * Description:		Set Variable Command
 */

#include FRONTEND_H
#include <macros.h>
#include <variable.h>
#include <nit/string.h>
#include <modules/base.h>

int base_cmd_set(char *env, char *str)
{
	char *name;
	void *window;
	string_t value;

	get_param_m(str, name, ' ');
	if (!(window = fe_next_widget()) && !(window = fe_first_widget()))
		return(-1);
	if (add_variable(name, create_string(str)))
		fe_print(window, create_string("Error setting variable.", name, str));
	else
		fe_print(window, create_string("Variable: %s <= %s", name, str));
	return(0);
}


