/*
 * Widget Name:		container.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; surface
 * Description:		Generic Container Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/lib/queue.h>
#include <stutter/lib/memory.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "container.h"
#include "window.h"

struct widget_type_s container_type = {
	"container:window",
	WT_CONTAINER,
	sizeof(struct container_s),
	(widget_init_t) container_init,
	(widget_release_t) container_release,
	(widget_refresh_t) container_refresh,
	(widget_print_t) container_print,
	(widget_clear_t) container_clear,
	(widget_read_t) container_read,
	(widget_control_t) container_control
};

int container_init(struct container_s *container)
{
	window_init(WINDOW_S(container));
	if (!(container->widgets = create_queue(0, (destroy_t) destroy_widget)))
		return(-1);
	return(0);
}

int container_release(struct container_s *container)
{
	if (container && container->widgets)
		destroy_queue(container->widgets);
	window_release(WINDOW_S(container));
	return(0);
 }

int container_refresh(struct container_s *container)
{
	struct widget_s *widget;

	// TODO what should this do?
	if (widget = (struct widget_s *) queue_current(container->widgets))
		widget_refresh_m(widget);
	else
		window_clear(WINDOW_S(container));
	return(0);
}

int container_print(struct container_s *container, const char *str, int len)
{
	return(0);
}

void container_clear(struct container_s *container)
{
//	surface_clear_m(container->surface, container->x, container->y, container->width, container->height);
}

int container_read(struct container_s *container, char *buffer, int max)
{
	return(-1);
}

int container_control(struct container_s *container, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SET_SURFACE: {
			struct queue_node_s *cur;
			struct surface_s *surface;

			surface = va_arg(va, struct surface_s *);
			WINDOW_S(container)->surface = surface;
			cur = queue_first_node(container->widgets);
			while (cur) {
				widget_control(WIDGET_S(cur->ptr), WCC_SET_SURFACE, surface);
				cur = queue_next_node(container->widgets);
			}
			return(0);
		}
		case WCC_SET_WINDOW: {
			struct queue_node_s *cur;

			window_control(WINDOW_S(container), cmd, va);
			// TODO what should this do about its child widgets?  Should it ignore them?
			cur = queue_first_node(container->widgets);
			while (cur) {
				widget_control(WIDGET_S(cur->ptr), WCC_SET_WINDOW, WINDOW_S(container)->x, WINDOW_S(container)->y, WINDOW_S(container)->width, WINDOW_S(container)->height);
				cur = queue_next_node(container->widgets);
			}
			return(0);
		}
		case WCC_ADD_WIDGET: {
			struct widget_s *widget;
			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			if (queue_append(container->widgets, widget))
				return(-1);
			widget->parent = WIDGET_S(container);
			return(0);
		}
		case WCC_INSERT_WIDGET: {
			struct widget_s *widget;
			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			if (queue_insert(container->widgets, widget))
				return(-1);
			widget->parent = WIDGET_S(container);
			return(0);
		}
		case WCC_REMOVE_WIDGET: {
			struct widget_s *widget;
			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			queue_remove(container->widgets, widget);
			if (!queue_current(container->widgets))
				queue_first(container->widgets);
			widget->parent = NULL;
			return(0);
		}
		case WCC_CURRENT_WIDGET: {
			va_list args;
			char *context;
			struct widget_s *current, **widget;

			args = va;
			widget = va_arg(va, struct widget_s **);
			context = va_arg(va, char *);
			if (!widget || !(current = (struct widget_s *) queue_current(container->widgets)))
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
			node = queue_current_node(container->widgets);
			cur = queue_first_node(container->widgets);
			while (cur) {
				if ((WIDGET_S(cur->ptr) == widget) || !widget_control_m(WIDGET_S(cur->ptr), cmd, args)) {
					container->widgets->current = cur;
					return(0);
				}
				cur = queue_next_node(container->widgets);
			}
			container->widgets->current = node;
			return(-1);
		}
		case WCC_NEXT_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			*widget = (struct widget_s *) queue_next(container->widgets);
			return(0);
		}
		case WCC_PREVIOUS_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			*widget = (struct widget_s *) queue_previous(container->widgets);
			return(0);
		}
		case WCC_FIRST_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			*widget = (struct widget_s *) queue_first(container->widgets);
			return(0);
		}
		case WCC_LAST_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			*widget = (struct widget_s *) queue_last(container->widgets);
			return(0);
		}
		default:
			return(window_control(WINDOW_S(container), cmd, va));
	}
	return(-1);
}


