/*
 * Command Name:	evaluate.c
 * Description:		Evaluate the command in env with args unless args
 * 			starts with the command prefix
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/string.h>
#include <stutter/frontend.h>

int base_cmd_evaluate(char *env, char *args)
{
	char *cmd;
	int pos = 0;

	if (!strncmp(args, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX))) {
		args = &args[strlen(BASE_COMMAND_PREFIX)];
		cmd = util_get_arg(args, &pos);
	}
	else if (!env)
		cmd = util_get_arg(args, &pos);
	else
		cmd = env;

	if (util_evaluate_command(cmd, &args[pos])) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNKNOWN_COMMAND, cmd)
	}
	return(0);
}


