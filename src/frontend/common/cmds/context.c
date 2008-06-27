/*
 * Command Name:	context.c
 * Description:		Key Context Command
 */

#include <stutter/utils.h>
#include <stutter/frontend/common/key.h>

int fe_common_cmd_context(char *env, char *args)
{
	char *context = NULL;

	if (*args != '\0')
		context = util_get_arg(args, NULL);
	if (fe_key_select_context(context))
		return(-1);
	return(0);
}


