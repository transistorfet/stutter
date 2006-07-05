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
	char buffer[STRING_SIZE];

	if (!(input = fe_current_widget("input", NULL)))
		return(-1);
	str = fe_read(input, buffer, STRING_SIZE);
	if (*str == '\0')
		return(-1);

	if (!strncmp(str, COMMAND_PREFIX, strlen(COMMAND_PREFIX))) {
		str = &str[strlen(COMMAND_PREFIX)];
		get_param_m(str, cmd, ' ');
	}
	else
		cmd = DEFAULT_COMMAND;

	if (!(var = index_variable(NULL, BASE_PARSE_PATH_VARIABLE, cmd)))
		var = find_variable(NULL, cmd);
	if (var && var->type->evaluate)
		var->type->evaluate(var->value, str);
	else
		fe_print(fe_current_widget("window", NULL), ERR_MSG_UNKNOWN_COMMAND);
	fe_clear(input);

	return(0);
}


