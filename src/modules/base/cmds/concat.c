/*
 * Command Name:	concat.c
 * Description:		Concatenate env and args and evaluate as a command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/frontend.h>

int base_cmd_concat(char *env, char *args)
{
	char *str, *cmd;
	char buffer[STRING_SIZE];

	if (env)
		strncpy(buffer, env, STRING_SIZE);
	else
		buffer[0] = '\0';
	str = strncat(buffer, args, STRING_SIZE);
	if (!strncmp(str, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX)))
		str = &str[strlen(BASE_COMMAND_PREFIX)];
	get_param_m(str, cmd, ' ');

	if (util_evaluate_command(cmd, str)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNKNOWN_COMMAND, cmd)
	}
	return(0);
}


