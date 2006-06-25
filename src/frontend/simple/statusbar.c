/*
 * Module Name:		statusbar.c
 * Version:		0.1
 * Module Requirements: type ; memory ; stringt ; variable ; terminal
 * Description:		Status Bar Widget
 */

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/string.h>
#include "terminal.h"
#include "statusbar.h"

/**
 * Allocate and initialize a statusbar structure given the intialization values.
 */
struct statusbar_s *create_statusbar(int height, char *name)
{
	struct statusbar_s *statusbar;

	if (!(statusbar = (struct statusbar_s *) memory_alloc(sizeof(struct statusbar_s))))
		return(NULL);
	statusbar->height = height;
	if (!(statusbar->status = find_variable(NULL, name)) || !statusbar->status->type->stringify) {
		memory_free(statusbar);
		return(NULL);
	}

	return(statusbar);
}

/**
 * Free all the resources used by the given statusbar structure.
 */
int destroy_statusbar(struct statusbar_s *statusbar)
{
	if (statusbar)
		memory_free(statusbar);
	return(0);
}

/**
 * Status Bar Refresh Function.  Redraw the status bar in the given frame
 * using the information from the given statusbar structure.
 */
int refresh_statusbar(struct statusbar_s *statusbar)
{
	int i;
	short width, height;
	char buffer[STRING_SIZE];

	width = terminal_width();
	height = terminal_height();

	terminal_set_attribs(SC_INVERSE, 0, 0);
	terminal_clear(0, height - statusbar->height - 1, width, statusbar->height);
	if ((i = statusbar->status->type->stringify(statusbar->status->value, buffer, STRING_SIZE)) >= 0) {
		if (i > width)
			buffer[width] = '\0';
		terminal_move(0, height - statusbar->height - 1);
		terminal_print(buffer, 0);
	}
	terminal_set_attribs(SC_NORMAL, 0, 0);
	return(0);
}


