/*
 * Command Name:	remove.c
 * Description:		Remove Variable Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/output.h>
#include <stutter/string.h>
#include <stutter/variable.h>
#include <stutter/modules/base.h>

int base_cmd_remove(char *env, char *args)
{
	char *name;

	name = util_get_arg(args, NULL);
	if ((*name == '\0') || remove_variable(NULL, NULL, name)) {
		OUTPUT_ERROR(BASE_ERR_REMOVE_FAILED, name);
		return(-1);
	}
	OUTPUT_STATUS(BASE_FMT_REMOVE, name);
	return(0);
}


