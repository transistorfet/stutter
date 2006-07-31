/*
 * Header Name:		statusbar.h
 * Version:		0.1
 * Description:		Status Bar Widget Header
 */

#ifndef _WIN32_STATUSBAR_H
#define _WIN32_STATUSBAR_H

#include <stutter/frontend/surface.h>

struct statusbar_s {
	int height;
	struct variable_s *status;
	struct surface_s *surface;
};

struct statusbar_s *create_statusbar(struct surface_s *, int, char *);
int destroy_statusbar(struct statusbar_s *);
int refresh_statusbar(struct statusbar_s *);

#endif


