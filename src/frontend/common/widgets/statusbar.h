/*
 * Header Name:		statusbar.h
 * Description:		Status Bar Widget Header
 */

#ifndef _COMMON_WIDGETS_STATUSBAR_H
#define _COMMON_WIDGETS_STATUSBAR_H

#include <stdarg.h>

#include <stutter/string.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "window.h"

struct statusbar_s {
	struct window_s window;
	string_t text;
};

#define STATUSBAR_S(widget)	((struct statusbar_s *) widget)

extern struct widget_type_s statusbar_type;

int statusbar_init(struct statusbar_s *, struct property_s *);
int statusbar_release(struct statusbar_s *);
int statusbar_refresh(struct statusbar_s *);
int statusbar_print(struct statusbar_s *, const char *, int);
void statusbar_clear(struct statusbar_s *);
int statusbar_read(struct statusbar_s *, char *, int);
int statusbar_control(struct statusbar_s *, int, va_list);

#endif


