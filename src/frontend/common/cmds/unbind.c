/*
 * Command Name:	unbind.c
 * Description:		Unbind Key Command
 */

#include <stdio.h>
#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/output.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/variable.h>
#include <stutter/frontend/common/key.h>

int fe_common_cmd_unbind(char *env, char *args)
{
	int pos = 0;
	char *context = NULL;
	int buffer[SMALL_STRING_SIZE];

	TRIM_WHITESPACE(args);
	if (*args == '-') {
		pos = 1;
		context = util_get_arg(&args[1], &pos);
	}

	if ((util_convert_key(&args[pos], buffer, SMALL_STRING_SIZE) < 0) || fe_key_unbind(context, buffer)) {
		OUTPUT_ERROR(FE_ERR_UNBINDING_FAILED, &args[pos]);
		return(-1);
	}
	OUTPUT_STATUS(FE_FMT_UNBIND, &args[pos]);
	return(0);
}


