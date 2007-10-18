/*
 * Command Name:	insert.c
 * Description:		Echo Command
 */


#include <stutter/memory.h>
#include <stutter/frontend.h>
#include <stutter/frontend/widget.h>

int fe_common_cmd_insert(struct widget_s *widget, char *args)
{
	if ((*args == '\0') || (!widget && !(widget = fe_get_focus("input"))))
		return(-1);
	widget_control(widget, WCC_INSERT_CHAR, args[0]);
	return(0);
}


