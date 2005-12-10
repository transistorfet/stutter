/*
 * Module Name:		statusbar.c
 * Version:		0.1
 * Module Requirements: type ; variable ; screen ; string
 * Description:		Status Bar Widget
 */

#include CONFIG_H
#include <type.h>
#include <variable.h>
#include <nit/string.h>
#include <nit/memory.h>
#include <nit/screen.h>
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
int refresh_statusbar(struct statusbar_s *statusbar, struct screen_s *screen)
{
	int width;
	string_t str;

	width = screen_width(screen);
	screen_set_attrib(screen, SC_INVERSE);
	screen_clear(screen, 0, screen_height(screen) - statusbar->height - 1, width, statusbar->height);
	if (str = statusbar->status->type->stringify(statusbar->status->value)) {
		if (strlen(str) > width)
			str[width] = '\0';
		screen_move(screen, 0, screen_height(screen) - statusbar->height - 1);
		screen_print(screen, str, 0);
		destroy_string(str);
	}
	screen_set_attrib(screen, 0);
	return(0);
}


