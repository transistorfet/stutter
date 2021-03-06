/*
 * Command Name:	concat.c
 * Description:		Concatenate env and args and evaluate as a command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/output.h>
#include <stutter/string.h>

int base_cmd_concat(char *env, char *args)
{
	int pos = 0;
	char *str, *cmd;
	char buffer[STRING_SIZE];

	if (env)
		strncpy(buffer, env, STRING_SIZE);
	else
		buffer[0] = '\0';
	str = strncat(buffer, args, STRING_SIZE);
	if (!strncmp(str, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX)))
		str = &str[strlen(BASE_COMMAND_PREFIX)];
	cmd = util_get_arg(str, &pos);

	if (util_evaluate_command(cmd, &str[pos])) {
		OUTPUT_ERROR(BASE_ERR_UNKNOWN_COMMAND, cmd);
	}
	return(0);
}


