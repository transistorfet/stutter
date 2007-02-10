/*
 * Command Name:	unbind.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; variable ; frontend ; string ; modbase
 * Description:		Unbind Key Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_unbind(char *env, char *args)
{
	void *value;
	struct type_s *type;
	char *context = NULL;
	int buffer[SMALL_STRING_SIZE];

	trim_whitespace_m(args);
	if (*args == '-')
		split_string_m(&args[1], context, args, ' ');

	if ((util_convert_key(args, buffer, SMALL_STRING_SIZE) < 0) || unbind_key(context, buffer)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNBINDING_FAILED, args);
		return(-1);
	}
	BASE_COMMAND_RESPONSE_JOINPOINT(BASE_FMT_UNBIND, args)
	return(0);
}


