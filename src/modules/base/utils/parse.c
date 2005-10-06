/*
 * Module Name:		parse.c
 * Version:		0.1
 * Module Requirements:	frontend ; command ; string
 * Description:		Command Parser
 */

#include <string.h>

#include CONFIG_H
#include <error.h>
#include <macros.h>
#include <command.h>
#include <frontend.h>
#include <nit/string.h>

int base_parse_command(char *env, int ch)
{
	char *str, *cmd;

	str = fe_get_input();
	if (*str == '\0')
		return(-1);

	if (!strncmp(str, COMMAND_PREFIX, strlen(COMMAND_PREFIX))) {
		str = &str[strlen(COMMAND_PREFIX)];
		get_param_m(str, cmd, ' ');
		if (evaluate_command(cmd, str) == ERR_COMMAND_NOT_FOUND)
			fe_print(fe_current_widget(), create_string(UNKNOWN_COMMAND));
	}
	else
		evaluate_command("", str);
	fe_clear_input();

	return(0);
}


