/*
 * Command Name:	remove.c
 * Description:		Remove Variable Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_remove(char *env, char *args)
{
	char *name;

	get_param_m(args, name, ' ');
	if (remove_variable(NULL, NULL, name)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_REMOVE_FAILED, name)
		return(-1);
	}
	BASE_COMMAND_RESPONSE_JOINPOINT(BASE_FMT_REMOVE, name)
	return(0);
}


