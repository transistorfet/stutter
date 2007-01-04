/*
 * Header Name:		window.h
 * Version:		0.1
 * Description:		Window Manager Header
 */

#ifndef _COMMON_WIDGETS_WINDOW_H
#define _COMMON_WIDGETS_WINDOW_H

#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>

struct window_s {
	struct widget_s widget;
	short x, y;
	short width, height;
	struct surface_s *surface;
};

#define WINDOW_S(widget)	((struct window_s *) widget)

extern struct widget_type_s window_type;

int window_init(struct window_s *);
int window_release(struct window_s *);
int window_refresh(struct window_s *);
int window_print(struct window_s *, const char *, int);
void window_clear(struct window_s *);
int window_read(struct window_s *, char *, int);
int window_control(struct window_s *, int, va_list);

#endif


