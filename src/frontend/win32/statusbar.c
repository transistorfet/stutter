/*
 * Module Name:		statusbar.c
 * Version:		0.1
 * Module Requirements: type ; memory ; stringt ; variable ; surface
 * Description:		Status Bar Widget
 */

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/string.h>
#include <stutter/frontend/surface.h>
#include "statusbar.h"

/**
 * Allocate and initialize a statusbar structure given the intialization values.
 */
struct statusbar_s *create_statusbar(struct surface_s *surface, int height, char *name)
{
	struct statusbar_s *statusbar;

	if (!(statusbar = (struct statusbar_s *) memory_alloc(sizeof(struct statusbar_s))))
		return(NULL);
	statusbar->height = height;
	statusbar->surface = surface;
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
	attrib_t attrib;
	short width, height;
	char buffer[STRING_SIZE];

	width = surface_get_width_m(statusbar->surface);
	height = surface_get_height_m(statusbar->surface);

	surface_control_m(statusbar->surface, SCC_GET_ATTRIB, &attrib);
	surface_control_m(statusbar->surface, SCC_MODIFY_ATTRIB, SA_INVERSE, -1, -1);
	surface_clear_m(statusbar->surface, 0, height - statusbar->height - 1, width, statusbar->height);
	if ((i = statusbar->status->type->stringify(statusbar->status->value, buffer, STRING_SIZE)) >= 0) {
		if (i > width)
			buffer[width] = '\0';
		surface_move_m(statusbar->surface, 0, height - statusbar->height - 1);
		surface_print_m(statusbar->surface, buffer, -1);
	}
	surface_control_m(statusbar->surface, SCC_SET_ATTRIB, &attrib);
	return(0);
}


