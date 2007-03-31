/*
 * Widget Name:		container.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; surface
 * Description:		Generic Container Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/memory.h>
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

int container_init(struct container_s *container, struct property_s *props)
{
	window_init(WINDOW_S(container), props);
	WIDGET_S(container)->bitflags |= WBF_NEEDS_REFRESH;
	if (container_widgets_init_list(container, 0))
		return(-1);
	return(0);
}

int container_release(struct container_s *container)
{
	if (container)
		container_widgets_release_list(container);
	window_release(WINDOW_S(container));
	return(0);
 }

int container_refresh(struct container_s *container)
{
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
			struct surface_s *surface;
			struct container_node_s *cur;

			surface = va_arg(va, struct surface_s *);
			WINDOW_S(container)->surface = surface;
			container_widgets_foreach(container, cur) {
				widget_control(cur->widget, WCC_SET_SURFACE, surface);
			}
			return(0);
		}
		case WCC_SHOW_WIDGET: {
			struct widget_s *widget;
			struct container_node_s *node;

			widget = va_arg(va, struct widget_s *);
			if (!(node = container_widgets_find_node(container, widget)))
				return(-1);
			container_widgets_set_current_node(container, node);
			return(0);
		}
		case WCC_SET_FOCUS: {
			struct widget_s *widget;
			struct container_node_s *node;

			widget = va_arg(va, struct widget_s *);
			if (!(node = container_widgets_find_node(container, widget)))
				return(-1);
			container_widgets_set_current_node(container, node);
			if (WIDGET_S(container)->parent)
				return(widget_control(WIDGET_S(container)->parent, WCC_SET_FOCUS, container));
			else if (WINDOW_S(container)->surface)
				surface_control_m(WINDOW_S(container)->surface, SCC_SET_FOCUS, NULL);
			return(0);
		}
		case WCC_GET_FOCUS: {
			struct container_node_s *node;

			if (!(node = container_widgets_current_node(container)) && !(node = container_widgets_first_node(container)))
				return(-1);
			return(widget_control_m(node->widget, WCC_GET_FOCUS, va));
		}
		case WCC_ADD_WIDGET: {
			struct widget_s *widget;
			struct container_node_s *node;

			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			if (!(node = container_widgets_create_node(0)))
				return(-1);
			node->widget = widget;
			if (container_widgets_append_node(container, node)) {
				/** We don't want to destroy the widget.  Just the node. */
				node->widget = NULL;
				container_widgets_destroy_node(container, node);
				return(-1);
			}
			widget_control(widget, WCC_SET_PARENT, container);
			return(0);
		}
		case WCC_INSERT_WIDGET: {
			struct widget_s *widget;
			struct container_node_s *node;

			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			if (!(node = container_widgets_create_node(0)))
				return(-1);
			node->widget = widget;
			if (container_widgets_insert_node(container, container_widgets_current_node(container), node)) {
				/** We don't want to destroy the widget.  Just the node. */
				node->widget = NULL;
				container_widgets_destroy_node(container, node);
				return(-1);
			}
			widget_control(widget, WCC_SET_PARENT, container);
			return(0);
		}
		case WCC_REMOVE_WIDGET: {
			struct widget_s *widget;
			struct container_node_s *node;

			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			if (!(node = container_widgets_find_node(container, widget)))
				return(-1);
			container_widgets_remove_node(container, node);
			/** We don't want to destroy the widget.  Just the node. */
			node->widget = NULL;
			container_widgets_destroy_node(container, node);
			if (!container_widgets_current_node(container))
				container_widgets_set_current_node(container, container_widgets_first_node(container));
			WIDGET_S(container)->bitflags |= WBF_FORCE_REFRESH;
			widget_control(widget, WCC_SET_PARENT, NULL);
			return(0);
		}

		case WCC_CURRENT_WIDGET: {
			struct widget_s **widget;
			struct container_node_s *cur;

			widget = va_arg(va, struct widget_s **);
			if (!(cur = container_widgets_current_node(container)) && !(cur = container_widgets_first_node(container)))
				return(-1);
			*widget = cur->widget;
			return(0);
		}
		case WCC_SELECT_WIDGET: {
			struct widget_s *widget;
			struct container_node_s *cur, *node;

			widget = va_arg(va, struct widget_s *);
			node = container_widgets_current_node(container);
			container_widgets_foreach(container, cur) {
				if (cur->widget == widget) {
					container_widgets_set_current_node(container, cur);
					WIDGET_S(container)->bitflags |= WBF_FORCE_REFRESH;
					return(0);
				}
			}
			container_widgets_set_current_node(container, node);
			WIDGET_S(container)->bitflags |= WBF_FORCE_REFRESH;
			return(-1);
		}
		case WCC_NEXT_WIDGET: {
			struct widget_s **widget;
			struct container_node_s *node;

			widget = va_arg(va, struct widget_s **);
			if (!(node = container_widgets_set_current_node(container, container_widgets_next_node(container_widgets_current_node(container)))))
				return(-1);
			WIDGET_S(container)->bitflags |= WBF_FORCE_REFRESH;
			if (widget)
				*widget = node->widget;
			return(0);
		}
		case WCC_PREVIOUS_WIDGET: {
			struct widget_s **widget;
			struct container_node_s *node;

			widget = va_arg(va, struct widget_s **);
			if (!(node = container_widgets_set_current_node(container, container_widgets_previous_node(container_widgets_current_node(container)))))
				return(-1);
			WIDGET_S(container)->bitflags |= WBF_FORCE_REFRESH;
			if (widget)
				*widget = node->widget;
			return(0);
		}
		case WCC_FIRST_WIDGET: {
			struct widget_s **widget;
			struct container_node_s *node;

			widget = va_arg(va, struct widget_s **);
			if (!(node = container_widgets_set_current_node(container, container_widgets_first_node(container))))
				return(-1);
			WIDGET_S(container)->bitflags |= WBF_FORCE_REFRESH;
			if (widget)
				*widget = node->widget;
			return(0);
		}
		case WCC_LAST_WIDGET: {
			struct widget_s **widget;
			struct container_node_s *node;

			widget = va_arg(va, struct widget_s **);
			if (!(node = container_widgets_set_current_node(container, container_widgets_last_node(container))))
				return(-1);
			WIDGET_S(container)->bitflags |= WBF_FORCE_REFRESH;
			if (widget)
				*widget = node->widget;
			return(0);
		}
		default:
			return(window_control(WINDOW_S(container), cmd, va));
	}
	return(-1);
}


