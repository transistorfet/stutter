/*
 * Widget Name:		container.c
 * Description:		Generic Container Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/memory.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/window.h>
#include <stutter/frontend/common/widgets/container.h>

struct fe_widget_type fe_container_type = { {
	OBJECT_TYPE_S(&fe_window_type),
	"fe_container",
	sizeof(struct fe_container),
	NULL,
	(object_init_t) fe_container_init,
	(object_release_t) fe_container_release },
	(fe_widget_write_t) fe_container_write,
	(fe_widget_read_t) fe_container_read,
	(fe_widget_refresh_t) fe_container_refresh,
	(fe_widget_clear_t) fe_container_clear,
	(fe_widget_control_t) fe_container_control
};

int fe_container_init(struct fe_container *container, const char *params, va_list va)
{
	if (fe_window_init(FE_WINDOW(container), params, va))
		return(-1);
	if (!(container->widgets = create_queue(0, (destroy_t) destroy_object)))
		return(-1);
	FE_WIDGET(container)->bitflags |= WBF_NEEDS_REFRESH;
	return(0);
}

void fe_container_release(struct fe_container *container)
{
	if (container)
		destroy_queue(container->widgets);
}

int fe_container_write(struct fe_container *container, const char *str, int len)
{
	return(0);
}

int fe_container_read(struct fe_container *container, char *buffer, int max)
{
	return(-1);
}

int fe_container_refresh(struct fe_container *container, struct fe_surface *surface)
{
	return(0);
}

void fe_container_clear(struct fe_container *container, struct fe_surface *surface)
{
//	FE_SURFACE_CLEAR(surface, FE_WINDOW(container)->x, FE_WINDOW(container)->y, FE_WINDOW(container)->width, FE_WINDOW(container)->height);
}

int fe_container_control(struct fe_container *container, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SHOW_WIDGET: {
/*
			struct fe_widget *widget;
			struct fe_container_node *node;

			widget = va_arg(va, struct fe_widget *);
			if (!(node = container_widgets_find_node(container, widget)))
				return(-1);
			container_widgets_set_current_node(container, node);
			FE_WIDGET(container)->bitflags |= WBF_FORCE_REFRESH;
*/
			return(0);
		}
		case WCC_SET_FOCUS: {
/*
			struct fe_widget *widget;
			struct fe_container_node *node;

			widget = va_arg(va, struct fe_widget *);
			if (!(node = container_widgets_find_node(container, widget)))
				return(-1);
			container_widgets_set_current_node(container, node);
			if (FE_WIDGET(container)->parent)
				return(fe_widget_control(FE_WIDGET(container)->parent, WCC_SET_FOCUS, container));
*/
			return(0);
		}
		case WCC_GET_FOCUS: {
			struct fe_widget *widget;

			if (!(widget = FE_WIDGET(QUEUE_CURRENT(container->widgets))) && !(widget = FE_WIDGET(QUEUE_FIRST(container->widgets))))
				return(-1);
			return(FE_WIDGET_CONTROL(widget, WCC_GET_FOCUS, va));
		}
		case WCC_ADD_WIDGET: {
			struct fe_widget *widget;

			widget = va_arg(va, struct fe_widget *);
			if (!widget)
				return(-1);
			if (queue_append(container->widgets, widget))
				return(-1);
			fe_widget_control(widget, WCC_SET_PARENT, container);
			return(0);
		}
		case WCC_INSERT_WIDGET: {
			struct fe_widget *widget;

			widget = va_arg(va, struct fe_widget *);
			if (!widget)
				return(-1);
			if (queue_insert(container->widgets, widget))
				return(-1);
			fe_widget_control(widget, WCC_SET_PARENT, container);
			return(0);
		}
		case WCC_REMOVE_WIDGET: {
/*
			struct fe_widget *widget;

			widget = va_arg(va, struct fe_widget *);
			if (!widget)
				return(-1);
			if (!(node = container_widgets_find_node(container, widget)))
				return(-1);
			container_widgets_remove_node(container, node);
			// We don't want to destroy the widget.  Just the node.
			node->widget = NULL;
			container_widgets_destroy_node(container, node);
			if (!container_widgets_current_node(container))
				container_widgets_set_current_node(container, container_widgets_first_node(container));
			FE_WIDGET(container)->bitflags |= WBF_FORCE_REFRESH;
			fe_widget_control(widget, WCC_SET_PARENT, NULL);
			return(0);
*/
		}
		case WCC_CURRENT_WIDGET: {
			struct fe_widget **widget;

			widget = va_arg(va, struct fe_widget **);
			if (!widget)
				return(-1);
			if (!(*widget = FE_WIDGET(QUEUE_CURRENT(container->widgets))))
				*widget = FE_WIDGET(QUEUE_FIRST(container->widgets));
			return(0);
		}
		case WCC_SELECT_WIDGET: {
/*
			struct fe_widget *widget;
			struct fe_container_node *cur, *node;

			widget = va_arg(va, struct fe_widget *);
			node = container_widgets_current_node(container);
			container_widgets_foreach(container, cur) {
				if (cur->widget == widget) {
					container_widgets_set_current_node(container, cur);
					FE_WIDGET(container)->bitflags |= WBF_FORCE_REFRESH;
					return(0);
				}
			}
			container_widgets_set_current_node(container, node);
			FE_WIDGET(container)->bitflags |= WBF_FORCE_REFRESH;
*/
			return(-1);
		}
		case WCC_NEXT_WIDGET: {
			struct fe_widget **widget;

			widget = va_arg(va, struct fe_widget **);
			if (!widget)
				return(-1);
			*widget = FE_WIDGET(QUEUE_NEXT(container->widgets));
			FE_WIDGET(container)->bitflags |= WBF_FORCE_REFRESH;
			return(0);
		}
		case WCC_PREVIOUS_WIDGET: {
			struct fe_widget **widget;

			widget = va_arg(va, struct fe_widget **);
			if (!widget)
				return(-1);
			*widget = FE_WIDGET(QUEUE_PREV(container->widgets));
			FE_WIDGET(container)->bitflags |= WBF_FORCE_REFRESH;
			return(0);
		}
		case WCC_FIRST_WIDGET: {
			struct fe_widget **widget;

			widget = va_arg(va, struct fe_widget **);
			if (!widget)
				return(-1);
			*widget = FE_WIDGET(QUEUE_FIRST(container->widgets));
			FE_WIDGET(container)->bitflags |= WBF_FORCE_REFRESH;
			return(0);
		}
		case WCC_LAST_WIDGET: {
			struct fe_widget **widget;

			widget = va_arg(va, struct fe_widget **);
			if (!widget)
				return(-1);
			*widget = FE_WIDGET(QUEUE_FIRST(container->widgets));
			FE_WIDGET(container)->bitflags |= WBF_FORCE_REFRESH;
			return(0);
		}
		default:
			return(fe_window_control(FE_WINDOW(container), cmd, va));
	}
	return(-1);
}


