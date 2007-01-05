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

static int region_split_insert(struct region_s *, struct region_node_s *, struct region_node_s *);
static int region_resize(struct region_s *, int);

int region_init(struct region_s *region)
{
	window_init(WINDOW_S(region));
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
	window_release(WINDOW_S(region));
	return(0);
 }

int region_refresh(struct region_s *region)
{
	queue_traverse_v(region->widgets, widgets,
		widget_refresh_m(cur->widget);
	);
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
		case WCC_SET_SURFACE: {
			struct region_node_s *cur;
			struct surface_s *surface;

			surface = va_arg(va, struct surface_s *);
			WINDOW_S(region)->surface = surface;
			cur = queue_first_v(region->widgets);
			while (cur) {
				widget_control(cur->widget, WCC_SET_SURFACE, surface);
				cur = queue_next_v(widgets, cur);
			}
			// TODO should this also cause a resize of everything?  at least if we are the root
			//	or is it the job of the code that sets the surface to also do a resize?
			return(0);
		}
		case WCC_SET_WINDOW: {
			int height;
			height = WINDOW_S(region)->height;
			window_control((struct window_s *) region, cmd, va);
			return(region_resize(region, WINDOW_S(region)->height - height));
		}
		case WCC_MODIFY_SIZE: {
			int width, height;
			struct widget_s *child;
			struct region_node_s *cur;
			widget_pos_t cur_pos, ch_pos;
			widget_size_t cur_min, cur_max, cur_size, ch_size;

			child = va_arg(va, struct widget_s *);
			width = va_arg(va, int);
			height = va_arg(va, int);
			if (!child && !WIDGET_S(region)->parent)
				return(-1);
			if (!child)
				return(widget_control(WIDGET_S(region)->parent, WCC_MODIFY_SIZE, region, width, height));
			else {
				// TODO this should cause a resize of the given widget
			}
			return(-1);
		}
		case WCC_GET_MIN_MAX_SIZE: {
			struct region_node_s *cur;
			widget_size_t *min, *max;
			widget_size_t ch_min, ch_max;

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
			cur = queue_first_v(region->widgets);
			while (cur) {
				widget_control(cur->widget, WCC_GET_MIN_MAX_SIZE, &ch_min, &ch_max);
				if (min) {
					min->width += ch_min.width;
					min->height += ch_min.height;
				}
				if (max) {
					max->width = (ch_max.width == -1) ? -1 : (max->width + ch_max.width);
					max->height = (ch_max.height == -1) ? -1 : (max->height + ch_max.height);
				}
				cur = queue_next_v(widgets, cur);
			}
			return(0);
		}
		case WCC_ADD_WIDGET:
		case WCC_INSERT_WIDGET: {
			widget_pos_t pos;
			struct widget_s *widget;
			struct region_node_s *node;
			widget_size_t size, min, max;

			widget = va_arg(va, struct widget_s *);
			if (!widget)
				return(-1);
			if (!(node = (struct region_node_s *) memory_alloc(sizeof(struct region_node_s))))
				return(-1);
			node->widget = widget;
			if (!queue_last_v(region->widgets)) {
				widget_control(widget, WCC_SET_WINDOW, WINDOW_S(region)->x, WINDOW_S(region)->y, WINDOW_S(region)->width, WINDOW_S(region)->height);
				queue_append_node_v(region->widgets, widgets, node);
			}
			else if (region_split_insert(region, (cmd == WCC_INSERT_WIDGET) ? region->current : queue_last_v(region->widgets), node)) {
				memory_free(node);
				return(-1);
			}
			widget_control(widget, WCC_SET_SURFACE, WINDOW_S(region)->surface);
			widget->parent = (struct widget_s *) region;
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
					widget->parent = NULL;
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
			return(window_control(WINDOW_S(region), cmd, va));
	}
	return(-1);
}

/*** Local Functions ***/

static int region_split_insert(struct region_s *region, struct region_node_s *node, struct region_node_s *new_node)
{
	widget_pos_t pos;
	struct region_node_s *cur;
	int min_height, total_height = 0;
	widget_size_t size, min, max, cur_min;

	widget_control(new_node->widget, WCC_GET_MIN_MAX_SIZE, &min, &max);
	min_height = min.height;
	cur = node;
	while (cur) {
		widget_control(cur->widget, WCC_GET_WINDOW, &pos, &size);
		widget_control(cur->widget, WCC_GET_MIN_MAX_SIZE, &cur_min, NULL);
		total_height += size.height;
		min_height += cur_min.height;
		if (total_height >= min_height)
			break;
		cur = queue_previous_v(widgets, cur);
	}
	if (!cur)
		return(-1);

	if (cur == node) {
		min_height = size.height / 2;
		if (min_height < cur_min.height)
			min_height = cur_min.height;
		if ((max.height != -1) && ((total_height - min_height) > max.height))
			min_height = total_height - max.height;
	}
	else
		min_height = total_height - (min_height - cur_min.height);
	widget_control(cur->widget, WCC_SET_WINDOW, pos.x, pos.y, WINDOW_S(region)->width, min_height);
	pos.y += min_height;
	min_height = total_height - min_height;
	while (cur && (cur != node)) {
		cur = queue_next_v(widgets, cur);
		widget_control(cur->widget, WCC_GET_MIN_MAX_SIZE, &cur_min, NULL);
		widget_control(cur->widget, WCC_SET_WINDOW, pos.x, pos.y, WINDOW_S(region)->width, cur_min.height);
		pos.y += cur_min.height;
		min_height -= cur_min.height;
	}
	// TODO can cur be NULL?
	widget_control(new_node->widget, WCC_SET_WINDOW, pos.x, pos.y, WINDOW_S(region)->width, min_height);
	queue_insert_node_v(region->widgets, widgets, node, new_node);
	return(0);
}

/**
 * Resize the widgets in the given region to occupy the whole of the region
 * given that the change in height of the region is diff_height.  If
 * diff_height is a positive number then the region has grown by that amount
 * and if it is a negative number then the region has shrunk by that amount.
 * Even if diff_height is 0, all widgets will be resized and the region's
 * width will be given as their width.  When a change to the height is
 * required, the bottom most widget will be grown or shrunk by that amount
 * up to its minimum or maximum.  If widget cannot change by diff_height, the
 * widget will be resized to its minimum or maximum and the next widget up will
 * be change by the remaining difference.  If all the widgets set to their
 * minimum are still too big for the region, the top most widgets will be set
 * to a size of 0.  If all widgets set to their maximum does not fill the
 * region then the top most widget will be expanded to fill the remaining
 * space.  On success a 0 is returned and non-zero on failure.
 */
static int region_resize(struct region_s *region, int diff_height)
{
	int height, pos;
	struct region_node_s *cur;
	widget_size_t size, min, max;

	pos = WINDOW_S(region)->height + WINDOW_S(region)->y;
	cur = queue_last_v(region->widgets);
	while (cur) {
		widget_control(cur->widget, WCC_GET_WINDOW, NULL, &size);
		widget_control(cur->widget, WCC_GET_MIN_MAX_SIZE, &min, &max);
		height = size.height + diff_height;
		if ((max.height != -1) && (height > max.height))
			height = max.height;
		else if (height < min.height)
			height = min.height;
		diff_height -= (height - size.height);
		pos -= height;
		if (pos < WINDOW_S(region)->y) {
			height -= (WINDOW_S(region)->y - pos);
			while (cur) {
				widget_control(cur->widget, WCC_SET_WINDOW, WINDOW_S(region)->x, WINDOW_S(region)->y, WINDOW_S(region)->width, height);
				height = 0;
				cur = queue_previous_v(widgets, cur);
			}
			return(0);
		}
		widget_control(cur->widget, WCC_SET_WINDOW, WINDOW_S(region)->x, pos, WINDOW_S(region)->width, height);
		cur = queue_previous_v(widgets, cur);
	}
	if ((diff_height > 0) && (cur = queue_first_v(region->widgets)))
		widget_control(cur->widget, WCC_SET_WINDOW, WINDOW_S(region)->x, WINDOW_S(region)->y, WINDOW_S(region)->width, height + diff_height);
	return(0);
}



/*

Behaviours:

Adding a new widget:
	The current widget is split in half.  If the splitting of the widget doesn't give at least the
	minimum size of the the current widget and the new widget together then we set the current widget
	to its minimum and attempt to split the widget above it assuming its size is its old size plus
	whatever amount the current widget is reduced by.  We proceed upwards until this is possible.  If
	we reach the top and cannot split the widget then everything will be crunched upwards making all
	widgets that can't fit even with their minmum size have a size of 0.

Making a widget bigger:
	The widget will grow bigger in the downward direction.  If all widgets below it are at their
	minimum and the height cannot be expanded any more, then the size will be that maximum.

Making a widget small:
	The widget will shrink upward making the widget below it bigger.  If that widget is at its maximum
	then it will pull the widget below it and so on until the height is satisfied or all the widgets
	below are at their maximum.  If all widgets below are at their maximum then the widget being resize
	will not be allowed to resize smaller.

*/

