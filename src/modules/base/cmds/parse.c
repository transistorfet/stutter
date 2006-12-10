/*
 * Command Name:	parse.c
 * Version:		0.1
 * Module Requirements:	utils ; frontend ; string
 * Description:		Parse Input as a Command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
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
		return(0);
	if (!strncmp(str, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX))) {
		str = &str[strlen(BASE_COMMAND_PREFIX)];
		get_param_m(str, cmd, ' ');
	}
	else
		cmd = BASE_NULL_COMMAND;

	if (util_evaluate_command(cmd, str)) {
		BASE_ERROR_JOINPOINT(ERR_MSG_UNKNOWN_COMMAND, cmd)
	}
	fe_clear(input);

	return(0);
}


