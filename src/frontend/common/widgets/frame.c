/*
 * Widget Name:		frame.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; surface
 * Description:		Frame Container Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/memory.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "frame.h"
#include "container.h"

struct widget_type_s frame_type = {
	"frame:container:window",
	WT_FRAME,
	sizeof(struct frame_s),
	(widget_init_t) frame_init,
	(widget_release_t) frame_release,
	(widget_refresh_t) frame_refresh,
	(widget_print_t) frame_print,
	(widget_clear_t) frame_clear,
	(widget_read_t) frame_read,
	(widget_control_t) frame_control
};

int frame_init(struct frame_s *frame)
{
	container_init(CONTAINER_S(frame));
	return(0);
}

int frame_release(struct frame_s *frame)
{
	container_release(CONTAINER_S(frame));
	return(0);
 }

int frame_refresh(struct frame_s *frame)
{
	struct container_node_s *node;

	if ((node = container_widgets_current_node(CONTAINER_S(frame))) && node->widget) {
		if ((node->widget->bitflags & WBF_NEEDS_REFRESH) || (WIDGET_S(frame)->bitflags & WBF_FORCE_REFRESH)) {
			widget_refresh_m(node->widget);
			WIDGET_S(frame)->bitflags &= ~WBF_FORCE_REFRESH;
		}
	}
	else
		window_clear(WINDOW_S(frame));
	return(0);
}

int frame_print(struct frame_s *frame, const char *str, int len)
{
	return(0);
}

void frame_clear(struct frame_s *frame)
{
//	surface_clear_m(frame->surface, frame->x, frame->y, frame->width, frame->height);
}

int frame_read(struct frame_s *frame, char *buffer, int max)
{
	return(-1);
}

int frame_control(struct frame_s *frame, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SET_SURFACE: {
			struct surface_s *surface;
			struct container_node_s *cur;

			surface = va_arg(va, struct surface_s *);
			WINDOW_S(frame)->surface = surface;
			container_widgets_foreach(CONTAINER_S(frame), cur) {
				widget_control(cur->widget, WCC_SET_SURFACE, surface);
			}
			// TODO should this also cause a resize of everything?  at least if we are the root
			return(0);
		}
		case WCC_SET_WINDOW: {
			struct container_node_s *cur;

			window_control(WINDOW_S(frame), cmd, va);
			container_widgets_foreach(CONTAINER_S(frame), cur) {
				widget_control(cur->widget, WCC_SET_WINDOW, WINDOW_S(frame)->x, WINDOW_S(frame)->y, WINDOW_S(frame)->width, WINDOW_S(frame)->height);
			}
			return(0);
		}
		case WCC_GET_MIN_MAX_SIZE: {
			widget_size_t *min, *max;
			widget_size_t ch_min, ch_max;
			struct container_node_s *cur;

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
			container_widgets_foreach(CONTAINER_S(frame), cur) {
				if (!widget_control(cur->widget, WCC_GET_MIN_MAX_SIZE, &ch_min, &ch_max)) {
					if (min) {
						if (ch_min.width > min->width)
							min->width = ch_min.width;
						if (ch_min.height > min->height)
							min->height = ch_min.height;
					}
					if (max) {
						if ((ch_max.width != -1) && (ch_max.width < max->width))
							max->width = ch_max.width;
						if ((ch_max.height != -1) && (ch_max.height < max->height))
							max->height = ch_max.height;
					}
				}
			}
			return(0);
		}
		case WCC_ADD_WIDGET:
		case WCC_INSERT_WIDGET: {
			struct widget_s *widget;

			if (container_control(CONTAINER_S(frame), cmd, va))
				return(-1);
			widget = va_arg(va, struct widget_s *);
			widget_control(widget, WCC_SET_WINDOW, WINDOW_S(frame)->x, WINDOW_S(frame)->y, WINDOW_S(frame)->width, WINDOW_S(frame)->height);
			return(0);
		}
		default:
			return(container_control(CONTAINER_S(frame), cmd, va));
	}
	return(-1);
}


