/*
 * Widget Name:		region.c
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
#include "region.h"
#include "window.h"

struct widget_type_s region_type = {
	"region:window",
	WT_REGION,
	sizeof(struct region_s),
	(widget_init_t) region_init,
	(widget_release_t) region_release,
	(widget_refresh_t) region_refresh,
	(widget_print_t) region_print,
	(widget_clear_t) region_clear,
	(widget_read_t) region_read,
	(widget_control_t) region_control
};

int region_init(struct region_s *region)
{
	region->current = NULL;
	queue_init_v(region->widgets);
	return(0);
}

int region_release(struct region_s *region)
{
	if (!region)
		return(-1);
	queue_destroy_v(region->widgets, widgets,
		destroy_widget(cur->widget);
		memory_free(cur);
	);
	return(0);
 }

int region_refresh(struct region_s *region)
{
	struct region_node_s *node;

/*
	if (widget = (struct widget_s *) queue_current(region->widgets))
		widget_refresh_m(widget);
	else
		window_clear((struct window_s *) region);
*/
	node = queue_first_v(region->widgets);
	while (node) {
		widget_refresh_m(node->widget);
		node = queue_next_v(widgets, node);
	}
	return(0);
}

int region_print(struct region_s *region, const char *str, int len)
{
	return(0);
}

void region_clear(struct region_s *region)
{
//	surface_clear_m(region->surface, region->x, region->y, region->width, region->height);
}

int region_read(struct region_s *region, char *buffer, int max)
{
	return(-1);
}

int region_control(struct region_s *region, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_ADD_WIDGET: {
			struct widget_s *widget;
			struct region_node_s *node;
			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			if (!(node = (struct region_node_s *) memory_alloc(sizeof(struct region_node_s))))
				return(-1);
			node->widget = widget;
			widget->parent = (struct widget_s *) region;
			queue_append_node_v(region->widgets, widgets, node);
			return(0);
		}
		case WCC_REMOVE_WIDGET: {
			struct widget_s *widget;
			struct region_node_s *node;
			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			node = queue_first_v(region->widgets);
			while (node) {
				if (node->widget == widget) {
					queue_remove_node_v(region->widgets, widgets, node);
					memory_free(node);
					return(0);
				}
				node = queue_next_v(widgets, node);
			}
			return(1);
		}
		case WCC_CURRENT_WIDGET: {
			va_list args;
			char *context;
			struct widget_s **widget;
			struct region_node_s *node;

			args = va;
			widget = va_arg(va, struct widget_s **);
			context = va_arg(va, char *);
			if (!widget)
				return(-1);
			node = region->current ? region->current : queue_first_v(region->widgets);
			while (node) {
				if (!widget_control_m(node->widget, cmd, args) && *widget)
					return(0);
				else if (strstr(node->widget->type->name, context)) {
					*widget = node->widget;
					return(0);
				}
				if (!(node = queue_next_v(widgets, node)) && region->current)
					node = queue_first_v(region->widgets);
				if (node == region->current)
					break;
			}
			*widget = NULL;
			return(0);
		}
		case WCC_SELECT_WIDGET: {
			va_list args;
			char *context;
			struct widget_s *widget;
			struct region_node_s *node;

			args = va;
			context = va_arg(va, char *);
			widget = va_arg(va, struct widget_s *);
			node = queue_first_v(region->widgets);
			while (node) {
				if ((node->widget == widget) || !widget_control_m(node->widget, cmd, args)) {
					region->current = node;
					return(0);
				}
				node = queue_next_v(widgets, node);
			}
			return(-1);
		}
		case WCC_NEXT_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			if (region->current)
				region->current = queue_next_v(widgets, region->current);
			*widget = region->current ? region->current->widget : NULL;
			return(0);
		}
		case WCC_PREVIOUS_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			if (region->current)
				region->current = queue_previous_v(widgets, region->current);
			*widget = region->current ? region->current->widget : NULL;
			return(0);
		}
		case WCC_FIRST_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			if (region->current)
				region->current = queue_first_v(region->widgets);
			*widget = region->current ? region->current->widget : NULL;
			return(0);
		}
		case WCC_LAST_WIDGET: {
			struct widget_s **widget;
			widget = va_arg(va, struct widget_s **);
			if (!widget)
				return(-1);
			if (region->current)
				region->current = queue_last_v(region->widgets);
			*widget = region->current ? region->current->widget : NULL;
			return(0);
		}
		default:
			return(window_control((struct window_s *) region, cmd, va));
	}
	return(-1);
}


