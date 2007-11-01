/*
 * Command Name:	context.c
 * Description:		Key Context Command
 */

#include <stutter/key.h>
#include <stutter/utils.h>
#include <stutter/modules/base.h>

int base_cmd_context(char *env, char *args)
{
	char *context = NULL;

	if (*args != '\0')
		context = util_get_arg(args, NULL);
	if (select_key_context(context))
		return(-1);
	return(0);
}


