/*
 * Type Name:		command.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; callback
 * Description:		Command Type
 */

#include <stdlib.h>

#include <type.h>
#include <variable.h>
#include <nit/callback.h>

static struct type_s *cmd_type;

int base_load_command(void)
{
	if (!(cmd_type = add_type("command", NULL, (evaluate_t) execute_callback, (destroy_t) destroy_callback)))
		return(-1);
	return(0);
}

int base_add_command(char *ns_name, char *var_name, callback_t func, void *ptr)
{
	if (!var_name || !func)
		return(-1);
	return(!add_variable(cmd_type, ns_name, var_name, (void *) create_callback(0, 0, NULL, func, ptr)));
}

