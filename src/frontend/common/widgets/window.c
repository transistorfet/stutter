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
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/window.h>

struct fe_widget_type fe_window_type = { {
	OBJECT_TYPE_S(&fe_widget_type),
	"fe_window",
	sizeof(struct fe_window),
	NULL,
	(object_init_t) fe_window_init,
	(object_release_t) NULL },
	(fe_widget_write_t) fe_window_write,
	(fe_widget_read_t) fe_window_read,
	(fe_widget_refresh_t) fe_window_refresh,
	(fe_widget_clear_t) fe_window_clear,
	(fe_widget_control_t) fe_window_control
};

int fe_window_init(struct fe_window *window, const char *params, va_list va)
{
	if (fe_widget_init(FE_WIDGET(window), params, va))
		return(-1);
	return(0);
}

int fe_window_write(struct fe_window *window, const char *str, int len)
{
	return(0);
}

int fe_window_read(struct fe_window *window, char *buffer, int max)
{
	return(-1);
}

int fe_window_refresh(struct fe_window *window, struct fe_surface *surface)
{
	return(0);
}

void fe_window_clear(struct fe_window *window, struct fe_surface *surface)
{
	FE_SURFACE_CLEAR(surface, window->x, window->y, window->width, window->height);
}

int fe_window_control(struct fe_window *window, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SET_PARENT: {
			struct fe_widget *parent;

			parent = va_arg(va, struct fe_widget *);
			FE_WIDGET(window)->parent = parent;
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
			FE_WIDGET(window)->bitflags |= WBF_NEEDS_REFRESH;
			return(0);
		}
		case WCC_MODIFY_SIZE: {
			int width, height;
			struct fe_widget *child;

			child = va_arg(va, struct fe_widget *);
			width = va_arg(va, int);
			height = va_arg(va, int);
			if (!child && !FE_WIDGET(window)->parent)
				return(-1);
			if (!child)
				return(fe_widget_control(FE_WIDGET(window)->parent, WCC_MODIFY_SIZE, window, width, height));
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


