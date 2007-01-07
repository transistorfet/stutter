/*
 * Command Name:	parse.c
 * Version:		0.1
 * Module Requirements:	utils ; frontend ; string
 * Description:		Parse Input as a Command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/frontend.h>

int base_cmd_parse(char *env, char *args)
{
	void *input;
	char *str, *cmd;
	char buffer[FE_INPUT_BUFFER_SIZE];

	if (!(input = fe_current_widget("input", NULL)))
		return(-1);
	str = fe_read(input, buffer, FE_INPUT_BUFFER_SIZE);
	fe_clear(input);

	if (*str == '\0')
		return(0);
	if (!strncmp(str, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX))) {
		str = &str[strlen(BASE_COMMAND_PREFIX)];
		get_param_m(str, cmd, ' ');
	}
	else
		cmd = BASE_NULL_COMMAND;

	if (util_evaluate_command(cmd, str)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNKNOWN_COMMAND, cmd)
	}
	return(0);
}


