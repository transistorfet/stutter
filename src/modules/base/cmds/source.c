/*
 * Command Name:	source.c
 * Version:		0.1
 * Module Requirements:	utils ; frontend ; string
 * Description:		Parse a File as a List of Commands
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/frontend.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>

int base_cmd_source(char *env, char *args)
{
	FILE *fptr;
	char *str, *cmd;
	struct variable_s *var;
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

	while (str = fgets(buffer, STRING_SIZE, fptr)) {
		if (cmd = strpbrk(str, "\n\r"))
			*cmd = '\0';
		if (*str != '\0') {
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


