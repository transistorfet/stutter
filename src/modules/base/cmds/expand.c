/*
 * Command Name:	expand.c
 * Description:		Expand all variables and evaluate as a command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/output.h>
#include <stutter/string.h>

int base_cmd_expand(char *env, char *args)
{
	int pos = 0;
	char *str, *cmd;
	char buffer[LARGE_STRING_SIZE];

	if (util_expand_str(args, buffer, LARGE_STRING_SIZE) < 0)
		return(-1);
	str = buffer;
	if (!strncmp(str, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX)))
		str = &str[strlen(BASE_COMMAND_PREFIX)];
	cmd = util_get_arg(str, &pos);

	if (util_evaluate_command(cmd, &str[pos])) {
		OUTPUT_ERROR(BASE_ERR_UNKNOWN_COMMAND, cmd);
	}
	return(0);
}


