/*
 * Command Name:	source.c
 * Description:		Parse a File as a List of Commands
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/frontend.h>

int base_cmd_source(char *env, char *args)
{
	FILE *fptr;
	char *str, *cmd;
	char buffer[STRING_SIZE];

	get_param_m(args, str, ' ');
	// TODO replace this getenv with something that can get you a home dir on windows too
	if ((*str == '~') && (cmd = getenv("HOME"))) {
		snprintf(buffer, STRING_SIZE, "%s%s", cmd, &str[1]);
		str = buffer;
	}

	if (!(fptr = fopen(str, "r"))) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNABLE_TO_OPEN_FILE, cmd)
		return(-1);
	}

	while ((str = fgets(buffer, STRING_SIZE, fptr))) {
		trim_whitespace_m(str);
		if ((cmd = strpbrk(str, "\n\r")))
			*cmd = '\0';
		if ((*str != '\0') && (*str != '#')) {
			if (!strncmp(str, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX)))
				str = &str[strlen(BASE_COMMAND_PREFIX)];
			get_param_m(str, cmd, ' ');
			if (util_evaluate_command(cmd, str)) {
				BASE_ERROR_JOINPOINT(BASE_ERR_UNKNOWN_COMMAND, cmd)
			}
		}
	}
	fclose(fptr);

	return(0);
}


