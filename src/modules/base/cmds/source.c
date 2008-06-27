/*
 * Command Name:	source.c
 * Description:		Parse a File as a List of Commands
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/output.h>
#include <stutter/macros.h>
#include <stutter/string.h>

int base_cmd_source(char *env, char *args)
{
	int pos;
	FILE *fptr;
	char *str, *cmd;
	char buffer[STRING_SIZE];

	str = util_get_arg(args, NULL);
	// TODO replace this getenv with something that can get you a home dir on windows too
	if ((*str == '~') && (cmd = getenv("HOME"))) {
		snprintf(buffer, STRING_SIZE, "%s%s", cmd, &str[1]);
		str = buffer;
	}

	if (!(fptr = fopen(str, "r"))) {
		OUTPUT_ERROR(BASE_ERR_UNABLE_TO_OPEN_FILE, cmd);
		return(-1);
	}

	while ((str = fgets(buffer, STRING_SIZE, fptr))) {
		pos = 0;
		TRIM_WHITESPACE(str);
		if ((cmd = strpbrk(str, "\n\r")))
			*cmd = '\0';
		if ((*str != '\0') && (*str != '#')) {
			if (!strncmp(str, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX)))
				str = &str[strlen(BASE_COMMAND_PREFIX)];
			cmd = util_get_arg(str, &pos);
			if (util_evaluate_command(cmd, &str[pos])) {
				OUTPUT_ERROR(BASE_ERR_UNKNOWN_COMMAND, cmd);
			}
		}
	}
	fclose(fptr);

	return(0);
}


