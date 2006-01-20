/*
 * Module Name:		statusbar.c
 * Version:		0.1
 * Module Requirements: type ; memory ; stringt ; variable ; screen
 * Description:		Status Bar Widget
 */

#include CONFIG_H
#include <type.h>
#include <variable.h>
#include <lib/memory.h>
#include <lib/string.h>
#include "screen.h"
#include "statusbar.h"

/**
 * Allocate and initialize a statusbar structure given the intialization values.
 */
struct statusbar_s *create_statusbar(int height, char *ns, char *name)
{
	struct statusbar_s *statusbar;

	if (!(statusbar = (struct statusbar_s *) memory_alloc(sizeof(struct statusbar_s))))
		return(NULL);
	statusbar->height = height;
	if (!(statusbar->status = find_variable(NULL, ns, name)) || !statusbar->status->type->stringify) {
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
	string_t str;
	short width, height;

	width = screen_width();
	height = screen_height();

	screen_set_attribs(SC_INVERSE, 0, 0);
	screen_clear(0, height - statusbar->height - 1, width, statusbar->height);
	if (str = statusbar->status->type->stringify(statusbar->status->value)) {
		if (strlen(str) > width)
			str[width] = '\0';
		screen_move(0, height - statusbar->height - 1);
		screen_print(str, 0);
		destroy_string(str);
	}
	screen_set_attribs(SC_NORMAL, 0, 0);
	return(0);
}


