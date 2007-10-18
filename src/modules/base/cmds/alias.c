/*
 * Command Name:	alias.c
 * Description:		Create Alias Command
 */

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/macros.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_alias(char *env, char *args)
{
	char *name, *str;
	struct type_s *type;

	trim_whitespace_m(args);
	get_param_m(args, name, ' ');

	if (!(type = find_type("command")) || !type->create) {
		BASE_ERROR_JOINPOINT(BASE_ERR_VARIABLE_NOT_FOUND, "command")
		return(-1);
	}
	else if (!(str = create_string("%s", args)) || !(add_variable(NULL, type, name, 0, "callback,string", base_cmd_concat, str))) {
		BASE_ERROR_JOINPOINT(BASE_ERR_ALIAS_FAILED, name)
		return(-1);
	}
	BASE_COMMAND_RESPONSE_JOINPOINT(BASE_FMT_ALIAS, name, args)
	return(0);
}


