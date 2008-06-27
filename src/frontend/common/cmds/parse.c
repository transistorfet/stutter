/*
 * Command Name:	parse.c
 * Description:		Parse Input as a Command
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/output.h>
#include <stutter/string.h>
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>

int fe_common_cmd_parse(char *env, char *args)
{
	struct fe_widget *focus;
	char buffer[FE_INPUT_BUFFER_SIZE];

	if (!(focus = fe_get_focus(NULL)))
		return(-1);
	FE_WIDGET_READ(focus, buffer, FE_INPUT_BUFFER_SIZE);
	if (buffer[0] == '\0')
		return(0);
	// TODO passing a null surface here is bad
	FE_WIDGET_CLEAR(focus, NULL);

	if (util_evaluate_command(args, buffer))
		OUTPUT_ERROR(FE_ERR_UNKNOWN_COMMAND, args);
	return(0);
}


