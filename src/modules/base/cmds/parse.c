/*
 * Command Name:	parse.c
 * Description:		Parse Input as a Command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/frontend.h>

int base_cmd_parse(char *env, char *args)
{
	void *focus;
	char buffer[FE_INPUT_BUFFER_SIZE];

	if (!(focus = fe_get_focus(NULL)))
		return(-1);
	fe_read(focus, buffer, FE_INPUT_BUFFER_SIZE);
	if (buffer[0] == '\0')
		return(0);
	fe_clear(focus);

	if (util_evaluate_command(args, buffer)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNKNOWN_COMMAND, args)
	}
	return(0);
}


