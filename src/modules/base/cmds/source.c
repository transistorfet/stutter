/*
 * Command Name:	source.c
 * Version:		0.1
 * Module Requirements:	type ; frontend ; variable ; string
 * Description:		Parse a File as a List of Commands
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/variable.h>
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
		util_emit_str("error_general", NULL, "Error opening file: %s", str);
		return(-1);
	}

	while (str = fgets(buffer, STRING_SIZE, fptr)) {
		if (cmd = strpbrk(str, "\n\r"))
			*cmd = '\0';
		if (*str != '\0') {
			if (!strncmp(str, COMMAND_PREFIX, strlen(COMMAND_PREFIX)))
				str = &str[strlen(COMMAND_PREFIX)];
			get_param_m(str, cmd, ' ');
			if (!(var = index_variable(NULL, BASE_PARSE_PATH_VARIABLE, cmd)))
				var = find_variable(NULL, cmd);
			if (var && var->type->evaluate)
				var->type->evaluate(var->value, str);
			else
				fe_print(fe_current_widget("window", NULL), ERR_MSG_UNKNOWN_COMMAND);
		}
	}
	fclose(fptr);

	return(0);
}


