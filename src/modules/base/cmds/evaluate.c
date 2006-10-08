/*
 * Command Name:	evaluate.c
 * Version:		0.1
 * Module Requirements:	utils ; frontend ; string
 * Description:		Concatenate env and args and evaluate as a command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>

int base_cmd_evaluate(char *env, char *args)
{
	char *str, *cmd;
	struct variable_s *var;
	char buffer[STRING_SIZE];

	strncpy(buffer, env, STRING_SIZE);
	str = strncat(buffer, args, STRING_SIZE);
	if (!strncmp(str, COMMAND_PREFIX, strlen(COMMAND_PREFIX)))
		str = &str[strlen(COMMAND_PREFIX)];
	get_param_m(str, cmd, ' ');

	if (util_execute_command(cmd, str))
		util_emit_str("base.error", NULL, ERR_MSG_UNKNOWN_COMMAND, cmd);
	return(0);
}

