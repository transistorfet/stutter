/*
 * Widget Name:		frame.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; surface
 * Description:		Frame Container Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/lib/queue.h>
#include <stutter/lib/memory.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "frame.h"
#include "window.h"

struct widget_type_s frame_type = {
	"frame:window",
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
	if (!(frame->widgets = create_queue(0, (destroy_t) destroy_widget)))
		return(-1);
	return(0);
}

int frame_release(struct frame_s *frame)
{
	if (frame && frame->widgets)
		destroy_queue(frame->widgets);
	return(0);
 }

int frame_refresh(struct frame_s *frame)
{
	struct widget_s *widget;

	if (widget = (struct widget_s *) queue_current(frame->widgets))
		widget_refresh_m(widget);
	else
		window_clear((struct window_s *) frame);
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
		case WCC_ADD_WIDGET: {
			struct widget_s *widget;
			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			if (queue_append(frame->widgets, widget))
				return(-1);
			widget->parent = (struct widget_s *) frame;
			return(0);
		}
		case WCC_REMOVE_WIDGET: {
			struct widget_s *widget;
			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			queue_remove(frame->widgets, widget);
			if (!queue_current(frame->widgets))
				queue_first(frame->widgets);
			return(0);
		}
		case WCC_CURRENT_WIDGET: {
			va_list args;
			char *context;
			struct widget_s *current, **widget;

			args = va;
			widget = va_arg(va, struct widget_s **);
			context = va_arg(va, char *);
			if (!widget || !(current = (struct widget_s *) queue_current(frame->widgets)))
				return(-1);
			if (!widget_control_m(current, cmd, args) && *widget)
				return(0);
			else if (strstr(current->type->name, context))
				*widget = current;
			else
				*widget = NULL;
			return(0);
		}
		case WCC_SELECT_WIDGET: {
			va_list args;
			char *context;
			struct widget_s *widget;
			struct queue_node_s *cur, *node;

			args = va;
			context = va_arg(va, char *);
			widget = va_arg(va, struct widget_s *);
			node = queue_current_node(frame->widgets);
			cur = queue_first_node(frame->widgets);
			while (cur) {
				if ((((struct widget_s *) cur->ptr) == widget) || !widget_control_m((struct widget_s *) cur->ptr, cmd, args)) {
					frame->widgets->current = cur;
					return(0);
				}
				cur = queue_next_node(frame->widgets);
			}
			frame->widgets->current = node;
			return(-1);
		}
		case WCC_NEXT_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			*widget = (struct widget_s *) queue_next(frame->widgets);
			return(0);
		}
		case WCC_PREVIOUS_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			*widget = (struct widget_s *) queue_previous(frame->widgets);
			return(0);
		}
		case WCC_FIRST_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			*widget = (struct widget_s *) queue_first(frame->widgets);
			return(0);
		}
		case WCC_LAST_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			*widget = (struct widget_s *) queue_last(frame->widgets);
			return(0);
		}
		default:
			return(window_control((struct window_s *) frame, cmd, va));
	}
	return(-1);
}


