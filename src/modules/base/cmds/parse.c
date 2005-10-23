/*
 * Command Name:	parse.c
 * Version:		0.1
 * Module Requirements:	type ; frontend ; variable ; string
 * Description:		Parse Input as a Command
 */

#include <string.h>

#include CONFIG_H
#include <type.h>
#include <macros.h>
#include <variable.h>
#include <frontend.h>
#include <nit/string.h>

int base_cmd_parse(char *env, char *args)
{
	char *str, *cmd, *ns;
	struct variable_s *var;

	str = fe_get_input();
	if (*str == '\0')
		return(-1);

	if (!strncmp(str, COMMAND_PREFIX, strlen(COMMAND_PREFIX))) {
		str = &str[strlen(COMMAND_PREFIX)];
		get_param_m(str, cmd, ' ');
		get_prefix_m(cmd, ns, ':');
		if ((var = find_variable(NULL, ns, cmd)) && var->type->evaluate)
			var->type->evaluate(var->value, str);
		else
			fe_print(fe_current_widget(), create_string(UNKNOWN_COMMAND));
	}
	else if ((var = find_variable(NULL, NULL, DEFAULT_COMMAND)) && var->type->evaluate)
		var->type->evaluate(var->value, str);
	fe_clear_input();

	return(0);
}


