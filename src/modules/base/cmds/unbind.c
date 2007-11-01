/*
 * Command Name:	unbind.c
 * Description:		Unbind Key Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/key.h>
#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_unbind(char *env, char *args)
{
	int pos = 0;
	char *context = NULL;
	int buffer[SMALL_STRING_SIZE];

	TRIM_WHITESPACE(args);
	if (*args == '-') {
		pos = 1;
		context = util_get_arg(&args[1], &pos);
	}

	if ((util_convert_key(&args[pos], buffer, SMALL_STRING_SIZE) < 0) || unbind_key(context, buffer)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNBINDING_FAILED, &args[pos]);
		return(-1);
	}
	BASE_COMMAND_RESPONSE_JOINPOINT(BASE_FMT_UNBIND, &args[pos])
	return(0);
}


