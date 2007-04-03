/*
 * Command Name:	evaluate.c
 * Version:		0.1
 * Module Requirements:	utils ; frontend ; string
 * Description:		Evaluate the command in env with args unless args
 * 			starts with the command prefix
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/frontend.h>

int base_cmd_evaluate(char *env, char *args)
{
	char *str, *cmd;

	if (!strncmp(args, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX))) {
		args = &args[strlen(BASE_COMMAND_PREFIX)];
		get_param_m(args, cmd, ' ');
	}
	else if (!env) {
		get_param_m(args, cmd, ' ');
	}
	else
		cmd = env;

	if (util_evaluate_command(cmd, args)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNKNOWN_COMMAND, cmd)
	}
	return(0);
}


