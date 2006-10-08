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
	if ((*str == '~') && (cmd = getenv("HOME"))) {
		snprintf(buffer, STRING_SIZE, "%s%s", cmd, &str[1]);
		str = buffer;
	}

	if (!(fptr = fopen(str, "r"))) {
		util_emit_str("base.error", NULL, ERR_MSG_UNABLE_TO_OPEN_FILE, str);
		return(-1);
	}

	while (str = fgets(buffer, STRING_SIZE, fptr)) {
		if (cmd = strpbrk(str, "\n\r"))
			*cmd = '\0';
		if (*str != '\0') {
			if (!strncmp(str, COMMAND_PREFIX, strlen(COMMAND_PREFIX)))
				str = &str[strlen(COMMAND_PREFIX)];
			get_param_m(str, cmd, ' ');
			if (util_execute_command(cmd, str))
				util_emit_str("base.error", NULL, ERR_MSG_UNKNOWN_COMMAND, cmd);
		}
	}
	fclose(fptr);

	return(0);
}


