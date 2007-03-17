/*
 * Header Name:		container.h
 * Version:		0.1
 * Description:		Generic Container Widget Header
 */

#ifndef _COMMON_WIDGETS_CONTAINER_H
#define _COMMON_WIDGETS_CONTAINER_H

#include <stutter/queue.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "window.h"

struct container_node_s {
	queue_node_v(container_node_s) widgets;
	struct widget_s *widget;
};

struct container_s {
	struct window_s window;
	queue_list_v(container_node_s) widgets;
};

#define CONTAINER_S(widget)	((struct container_s *) widget)

extern struct widget_type_s container_type;

#define container_release_node(widgets, node)		( node ? destroy_widget(node->widget) : 0 )
#define container_compare_node(node, ptr)		(node->widget == ptr)
DEFINE_QUEUE(container_widgets, struct container_s, struct container_node_s, widgets, container_release_node, container_compare_node)
#define container_widgets_foreach(container, cur)	queue_foreach_v(container->widgets, widgets, cur)

int container_init(struct container_s *, struct property_s *);
int container_release(struct container_s *);
int container_refresh(struct container_s *);
int container_print(struct container_s *, const char *, int);
void container_clear(struct container_s *);
int container_read(struct container_s *, char *, int);
int container_control(struct container_s *, int, va_list);

#endif

