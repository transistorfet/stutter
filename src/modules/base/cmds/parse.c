/*
 * Command Name:	parse.c
 * Version:		0.1
 * Module Requirements:	type ; frontend ; variable ; string
 * Description:		Parse Input as a Command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>

int base_cmd_parse(char *env, char *args)
{
	void *input;
	char *str, *cmd;
	struct variable_s *var;

	if (!(input = fe_current_widget("input", NULL)))
		return(-1);
	str = fe_read(input);
	if (*str == '\0')
		return(-1);

	if (!strncmp(str, COMMAND_PREFIX, strlen(COMMAND_PREFIX))) {
		str = &str[strlen(COMMAND_PREFIX)];
		get_param_m(str, cmd, ' ');
		if ((var = find_variable(NULL, cmd)) && var->type->evaluate)
			var->type->evaluate(var->value, str);
		else
			fe_print(fe_current_widget("window", NULL), create_string(UNKNOWN_COMMAND));
	}
	else if ((var = find_variable(NULL, DEFAULT_COMMAND)) && var->type->evaluate)
		var->type->evaluate(var->value, str);
	fe_clear(input);

	return(0);
}


