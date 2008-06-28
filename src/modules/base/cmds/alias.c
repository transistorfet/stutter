/*
 * Command Name:	alias.c
 * Description:		Create Alias Command
 */

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/object.h>
#include <stutter/output.h>
#include <stutter/variable.h>
#include <stutter/modules/base/base.h>

int base_cmd_alias(char *env, char *args)
{
	int pos = 0;
	char *name, *str;
	struct object_type_s *type;

	name = util_get_arg(args, &pos);
	if (!(type = object_find_type("command", NULL))) {
		OUTPUT_ERROR(BASE_ERR_VARIABLE_NOT_FOUND, "command");
		return(-1);
	}
	// TODO this is dangerous because the string we create here will never be freed
	else if ((*name == '\0') || (&args[pos] == '\0') || !(str = create_string("%s", &args[pos]))
	    || !(add_variable(NULL, type, name, 0, "fs", base_cmd_concat, str))) {
		OUTPUT_ERROR(BASE_ERR_ALIAS_FAILED, name);
		return(-1);
	}
	OUTPUT_STATUS(BASE_FMT_ALIAS, name, &args[pos]);
	return(0);
}


