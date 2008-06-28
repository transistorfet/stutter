/*
 * Command Name:	get.c
 * Description:		Get Variable Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/output.h>
#include <stutter/string.h>
#include <stutter/object.h>
#include <stutter/variable.h>
#include <stutter/modules/base/base.h>

int base_cmd_get(char *env, char *args)
{
	int i, ret;
	char *name;
	char buffer[STRING_SIZE];

	name = util_get_arg(args, NULL);
	i = snprintf(buffer, STRING_SIZE, "Variable: %s = ", name);
	ret = stringify_variable(NULL, name, &buffer[i], STRING_SIZE - i);

	if (ret < 0) {
		OUTPUT_ERROR(BASE_ERR_STRINGIFY_FAILED, name);
		return(-1);
	}
	else {
		OUTPUT_STATUS("%s", buffer);
	}
	return(0);
}


