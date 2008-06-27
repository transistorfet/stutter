/*
 * Widget Name:		region.c
 * Description:		Region Container Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/memory.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/region.h>
#include <stutter/frontend/common/widgets/container.h>

struct fe_widget_type fe_region_type = { {
	OBJECT_TYPE_S(&fe_container_type),
	"fe_region",
	sizeof(struct fe_region),
	NULL,
	(object_init_t) fe_region_init,
	(object_release_t) fe_region_release },
	(fe_widget_write_t) fe_region_write,
	(fe_widget_read_t) fe_region_read,
	(fe_widget_refresh_t) fe_region_refresh,
	(fe_widget_clear_t) fe_region_clear,
	(fe_widget_control_t) fe_region_control
};

static int fe_region_split_insert(struct fe_region *, struct queue_entry_s *, struct queue_entry_s *);
static int fe_region_resize(struct fe_region *, int);

int fe_region_init(struct fe_region *region, const char *params, va_list va)
{
	if (fe_container_init(FE_CONTAINER(region), params, va))
		return(-1);
	return(0);
}

void fe_region_release(struct fe_region *region)
{
	fe_container_release(FE_CONTAINER(region));
}

int fe_region_write(struct fe_region *region, const char *str, int len)
{
	return(0);
}

int fe_region_read(struct fe_region *region, char *buffer, int max)
{
	return(-1);
}

int fe_region_refresh(struct fe_region *region, struct fe_surface *surface)
{
	struct queue_entry_s *entry;

	entry = QUEUE_CURRENT_ENTRY(FE_CONTAINER(region)->widgets);
	QUEUE_FOREACH(FE_CONTAINER(region)->widgets) {
		if ((FE_WIDGET(QUEUE_CURRENT(FE_CONTAINER(region)->widgets))->bitflags & WBF_NEEDS_REFRESH) || (FE_WIDGET(region)->bitflags & WBF_FORCE_REFRESH)) {
			FE_WIDGET_REFRESH(QUEUE_CURRENT(FE_CONTAINER(region)->widgets), surface);
			FE_WIDGET(region)->bitflags &= ~WBF_FORCE_REFRESH;
		}
	}
	QUEUE_SET_CURRENT_ENTRY(FE_CONTAINER(region)->widgets, entry);
	return(0);
}

void fe_region_clear(struct fe_region *region, struct fe_surface *surface)
{
//	FE_SURFACE_CLEAR(surface, FE_WINDOW(region)->x, FE_WINDOW(region)->y, FE_WINDOW(region)->width, FE_WINDOW(region)->height);
}

int fe_region_control(struct fe_region *region, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SET_WINDOW: {
			int height;
			height = FE_WINDOW(region)->height;
			fe_window_control(FE_WINDOW(region), cmd, va);
			return(fe_region_resize(region, FE_WINDOW(region)->height - height));
		}
		case WCC_MODIFY_SIZE: {
			int width, height;
			struct fe_widget *child;

			child = va_arg(va, struct fe_widget *);
			width = va_arg(va, int);
			height = va_arg(va, int);
			if (!child && !FE_WIDGET(region)->parent)
				return(-1);
			if (!child)
				return(fe_widget_control(FE_WIDGET(region)->parent, WCC_MODIFY_SIZE, region, width, height));
			else {
				// TODO this should cause a resize of the given widget
			}
			return(-1);
		}
		case WCC_GET_MIN_MAX_SIZE: {
			widget_size_t *min, *max;
			struct queue_entry_s *entry;
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
			entry = QUEUE_CURRENT_ENTRY(FE_CONTAINER(region)->widgets);
			QUEUE_FOREACH(FE_CONTAINER(region)->widgets) {
				fe_widget_control(QUEUE_CURRENT(FE_CONTAINER(region)->widgets), WCC_GET_MIN_MAX_SIZE, &ch_min, &ch_max);
				if (min) {
					min->width += ch_min.width;
					min->height += ch_min.height;
				}
				if (max) {
					max->width = (ch_max.width == -1) ? -1 : (max->width + ch_max.width);
					max->height = (ch_max.height == -1) ? -1 : (max->height + ch_max.height);
				}
			}
			QUEUE_SET_CURRENT_ENTRY(FE_CONTAINER(region)->widgets, entry);
			return(0);
		}
		case WCC_SHOW_WIDGET: {
			return(0);
		}
		case WCC_SET_FOCUS: {
			struct fe_widget *widget;

			widget = va_arg(va, struct fe_widget *);
			region->focus = widget;
			if (FE_WIDGET(region)->parent)
				return(fe_widget_control(FE_WIDGET(region)->parent, WCC_SET_FOCUS, region));
			return(0);
		}
		case WCC_GET_FOCUS: {
			if (!(region->focus) && (!(region->focus = FE_WIDGET(QUEUE_FIRST(FE_CONTAINER(region)->widgets)))))
				return(-1);
			return(FE_WIDGET_CONTROL(region->focus, WCC_GET_FOCUS, va));
		}
		case WCC_ADD_WIDGET:
		case WCC_INSERT_WIDGET: {
			struct fe_widget *widget;

			widget = va_arg(va, struct fe_widget *);
			if (!widget)
				return(-1);
			if (!QUEUE_LAST(FE_CONTAINER(region)->widgets)) {
				fe_widget_control(widget, WCC_SET_WINDOW, FE_WINDOW(region)->x, FE_WINDOW(region)->y, FE_WINDOW(region)->width, FE_WINDOW(region)->height);
				queue_append(FE_CONTAINER(region)->widgets, widget);
			}
			else if (fe_region_split_insert(region, (cmd == WCC_INSERT_WIDGET) ? QUEUE_CURRENT_ENTRY(FE_CONTAINER(region)->widgets) : QUEUE_LAST(FE_CONTAINER(region)->widgets), widget))
				return(-1);
			fe_widget_control(widget, WCC_SET_PARENT, region);
			return(0);
		}
		case WCC_REMOVE_WIDGET: {
			if (fe_container_control(FE_CONTAINER(region), cmd, va))
				return(-1);
			// TODO do a resize
			return(0);
		}
		default:
			return(fe_container_control(FE_CONTAINER(region), cmd, va));
	}
	return(-1);
}

/*** Local Functions ***/

static int fe_region_split_insert(struct fe_region *region, struct queue_entry_s *node, struct queue_entry_s *new_node)
{
/*
	widget_pos_t pos;
	struct fe_container_node *cur;
	int min_height, total_height = 0;
	widget_size_t size, min, max, cur_min;

	fe_widget_control(new_node->widget, WCC_GET_MIN_MAX_SIZE, &min, &max);
	min_height = min.height;
	cur = node;
	while (cur) {
		fe_widget_control(cur->widget, WCC_GET_WINDOW, &pos, &size);
		fe_widget_control(cur->widget, WCC_GET_MIN_MAX_SIZE, &cur_min, NULL);
		total_height += size.height;
		min_height += cur_min.height;
		if (total_height >= min_height)
			break;
		cur = container_widgets_previous_node(cur);
	}
	// TODO can we still add the widget even though it wont fit?
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
	fe_widget_control(cur->widget, WCC_SET_WINDOW, pos.x, pos.y, FE_WINDOW(region)->width, min_height);
	pos.y += min_height;
	min_height = total_height - min_height;
	while (cur && (cur != node)) {
		cur = container_widgets_next_node(cur);
		fe_widget_control(cur->widget, WCC_GET_MIN_MAX_SIZE, &cur_min, NULL);
		fe_widget_control(cur->widget, WCC_SET_WINDOW, pos.x, pos.y, FE_WINDOW(region)->width, cur_min.height);
		pos.y += cur_min.height;
		min_height -= cur_min.height;
	}
	// TODO can cur be NULL?
	fe_widget_control(new_node->widget, WCC_SET_WINDOW, pos.x, pos.y, FE_WINDOW(region)->width, min_height);
	container_widgets_insert_node(FE_CONTAINER(region), node, new_node);
	return(0);
*/
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
static int fe_region_resize(struct fe_region *region, int diff_height)
{
/*
	int height, pos;
	struct fe_container_node *cur;
	widget_size_t size, min, max;

	pos = FE_WINDOW(region)->height + FE_WINDOW(region)->y;
	cur = container_widgets_last_node(FE_CONTAINER(region));
	while (cur) {
		fe_widget_control(cur->widget, WCC_GET_WINDOW, NULL, &size);
		fe_widget_control(cur->widget, WCC_GET_MIN_MAX_SIZE, &min, &max);
		height = size.height + diff_height;
		if ((max.height != -1) && (height > max.height))
			height = max.height;
		else if (height < min.height)
			height = min.height;
		diff_height -= (height - size.height);
		pos -= height;
		if (pos < FE_WINDOW(region)->y) {
			height -= (FE_WINDOW(region)->y - pos);
			while (cur) {
				fe_widget_control(cur->widget, WCC_SET_WINDOW, FE_WINDOW(region)->x, FE_WINDOW(region)->y, FE_WINDOW(region)->width, height);
				height = 0;
				cur = container_widgets_previous_node(cur);
			}
			return(0);
		}
		fe_widget_control(cur->widget, WCC_SET_WINDOW, FE_WINDOW(region)->x, pos, FE_WINDOW(region)->width, height);
		cur = container_widgets_previous_node(cur);
	}
	if ((diff_height > 0) && (cur = container_widgets_first_node(FE_CONTAINER(region))))
		fe_widget_control(cur->widget, WCC_SET_WINDOW, FE_WINDOW(region)->x, FE_WINDOW(region)->y, FE_WINDOW(region)->width, height + diff_height);
	return(0);
*/
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

