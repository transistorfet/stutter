/*
 * Widget Name:		window.c
 * Version:		0.1
 * Module Requirements:	memory ; surface
 * Description:		Generic Window Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/lib/memory.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "window.h"

struct widget_type_s window_type = {
	"window",
	0,
	sizeof(struct window_s),
	(widget_init_t) window_init,
	(widget_release_t) window_release,
	(widget_refresh_t) window_refresh,
	(widget_print_t) window_print,
	(widget_clear_t) window_clear,
	(widget_read_t) window_read,
	(widget_control_t) window_control
};

int window_init(struct window_s *window)
{
	return(0);
}

int window_release(struct window_s *window)
{
	return(0);
}

int window_refresh(struct window_s *window)
{
	return(0);
}

int window_print(struct window_s *window, const char *str, int len)
{
	return(0);
}

void window_clear(struct window_s *window)
{
	surface_clear_m(window->surface, window->x, window->y, window->width, window->height);
}

int window_read(struct window_s *window, char *buffer, int max)
{
	return(-1);
}

int window_control(struct window_s *window, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_GET_SURFACE: {
			struct surface_s **surface;
			surface = va_arg(va, struct surface_s **);
			*surface = window->surface;
			return(0);
		}
		case WCC_SET_SURFACE: {
			struct surface_s *surface;
			surface = va_arg(va, struct surface_s *);
			window->surface = surface;
			return(0);
		}
		case WCC_GET_SIZE: {
			int *width, *height;
			width = va_arg(va, int *);
			height = va_arg(va, int *);
			if (width)
				*width = window->width;
			if (height)
				*height = window->height;
			return(0);
		}
		case WCC_MODIFY_SIZE: {
			int width, height;
			width = va_arg(va, int);
			height = va_arg(va, int);
			if (width != -1)
				window->width = width;
			if (height != -1)
				window->height = height;
			if (window->x >= width)
				window->x = width - 1;
			if (window->y >= height)
				window->y = height - 1;
			return(0);
		}
		case WCC_GET_POSITION: {
			int *x, *y;
			x = va_arg(va, int *);
			y = va_arg(va, int *);
			if (x)
				*x = window->x;
			if (y)
				*y = window->y;
			return(0);
		}
		case WCC_MODIFY_POSITION: {
			int x, y, width, height;
			x = va_arg(va, int);
			y = va_arg(va, int);
			width = surface_get_width_m(window->surface);
			height = surface_get_height_m(window->surface);
			if (x >= width)
				x = width - 1;
			if (y >= height)
				y = height - 1;
			if (x != -1)
				window->x = x;
			if (y != -1)
				window->y = y;
			return(0);
		}
		default:
			break;
	}
	return(-1);
}


