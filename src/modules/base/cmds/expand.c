/*
 * Command Name:	expand.c
 * Version:		0.1
 * Module Requirements:	utils ; frontend ; string
 * Description:		Expand all variables and evaluate as a command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>

int base_cmd_expand(char *env, char *args)
{
	char *str, *cmd;
	char buffer[STRING_SIZE];

	if (util_expand_str(args, buffer, STRING_SIZE) < 0)
		return(-1);
	str = buffer;
	if (!strncmp(str, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX)))
		str = &str[strlen(BASE_COMMAND_PREFIX)];
	get_param_m(str, cmd, ' ');

	if (util_evaluate_command(cmd, str)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNKNOWN_COMMAND, cmd)
	}
	return(0);
}


