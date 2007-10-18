/*
 * Widget Name:		window.c
 * Description:		Generic Window Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/memory.h>
#include <stutter/globals.h>
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

int window_init(struct window_s *window, struct property_s *props)
{
	int num;
	char *value;

	window->surface = NULL;
	if ((value = get_property(props, "width"))) {
		num = util_atoi(value, 10);
		window->width = num;
	}
	if ((value = get_property(props, "height"))) {
		num = util_atoi(value, 10);
		window->height = num;
	}
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
		case WCC_SET_PARENT: {
			struct widget_s *parent;

			parent = va_arg(va, struct widget_s *);
			WIDGET_S(window)->parent = parent;
			return(0);
		}
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
		case WCC_GET_WINDOW: {
			widget_pos_t *pos;
			widget_size_t *size;
			pos = va_arg(va, widget_pos_t *);
			size = va_arg(va, widget_size_t *);
			if (pos) {
				pos->x = window->x;
				pos->y = window->y;
			}
			if (size) {
				size->width = window->width;
				size->height = window->height;
			}
			return(0);
		}
		case WCC_SET_WINDOW: {
			window->x = va_arg(va, int);
			window->y = va_arg(va, int);
			window->width = va_arg(va, int);
			window->height = va_arg(va, int);
			WIDGET_S(window)->bitflags |= WBF_NEEDS_REFRESH;
			return(0);
		}
		case WCC_MODIFY_SIZE: {
			int width, height;
			struct widget_s *child;

			child = va_arg(va, struct widget_s *);
			width = va_arg(va, int);
			height = va_arg(va, int);
			if (!child && !WIDGET_S(window)->parent)
				return(-1);
			if (!child)
				return(widget_control(WIDGET_S(window)->parent, WCC_MODIFY_SIZE, window, width, height));
			return(-1);
		}
		case WCC_GET_MIN_MAX_SIZE: {
			widget_size_t *min, *max;
			min = va_arg(va, widget_size_t *);
			max = va_arg(va, widget_size_t *);
			if (min) {
				min->width = 1;
				min->height = 1;
			}
			if (max) {
				max->width = -1;
				max->height = -1;
			}
			return(0);
		}
		default:
			break;
	}
	return(-1);
}


