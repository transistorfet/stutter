/*
 * Command Name:	context.c
 * Version:		0.1
 * Module Requirements:	key ; modbase
 * Description:		Key Context Command
 */

#include <stutter/key.h>
#include <stutter/macros.h>
#include <stutter/modules/base.h>

int base_cmd_context(char *env, char *args)
{
	char *context = NULL;

	trim_whitespace_m(args);
	if (*args != '\0') {
		get_param_m(args, context, ' ');
	}
	if (select_key_context(context))
		return(-1);
	return(0);
}


