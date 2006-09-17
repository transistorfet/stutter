/*
 * Command Name:	insert.c
 * Version:		0.1
 * Module Requirements:	widget
 * Description:		Echo Command
 */


#include <stutter/frontend.h>
#include <stutter/lib/memory.h>
#include <stutter/frontend/widget.h>

int common_cmd_insert(struct widget_s *widget, char *args)
{
	if ((*args == '\0') || (!widget && !(widget = fe_current_widget("input", NULL))))
		return(-1);
	widget_control(widget, WCC_INSERT_CHAR, args[0]);
	return(0);
}


