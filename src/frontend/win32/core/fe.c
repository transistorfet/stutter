/*
 * Module Name:		fe.c
 * Version:		0.1
 * Module Requirements:	queue ; signal ; widget ; surface ; layout
 * Description:		Windows Frontend
 */


#include <stdio.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/signal.h>
#include <stutter/globals.h>
#include <stutter/frontend/common.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/common/layout.h>

extern struct surface_type_s *terminal_type;

static struct queue_s *surface_list;

extern int init_terminal(void);
extern int release_terminal(void);

static int add_builtin_layouts(void);
static int fe_add_surface(struct surface_s *);
static int fe_destroy_surface(struct surface_s *);
static int fe_handle_purge_surface(void *, struct surface_s *);

int init_frontend(void)
{
	if (!(surface_list = create_queue(0, (destroy_t) fe_destroy_surface)))
		return(-1);
	if (init_widget())
		return(-1);
	if (init_layout())
		return(-1);
	if (init_terminal())
		return(-1);

	// TODO this should be moved someplace else
	layout_register_type("window", LAYOUT_RT_WIDGET, (layout_create_t) generate_widget, &window_type);
	layout_register_type("text", LAYOUT_RT_WIDGET, (layout_create_t) generate_widget, &text_type);
	layout_register_type("frame", LAYOUT_RT_WIDGET, (layout_create_t) generate_widget, &frame_type);
	layout_register_type("input", LAYOUT_RT_WIDGET, (layout_create_t) generate_widget, &input_type);
	layout_register_type("statusbar", LAYOUT_RT_WIDGET, (layout_create_t) generate_widget, &statusbar_type);
	layout_register_type("region", LAYOUT_RT_WIDGET, (layout_create_t) generate_widget, &region_type);
	add_builtin_layouts();

	if (!(fe_create_widget("fe", "root", "root", NULL)))
		return(-1);
	return(0);
}

int release_frontend(void)
{
	destroy_queue(surface_list);
	release_terminal();
	release_layout();
	release_widget();
	return(0);
}

void *fe_create_widget(char *ns, char *type, char *id, void *parent)
{
	struct widget_s *widget;
	struct surface_s *surface;

	if (!parent) {
		if (!(surface = (struct surface_s *) layout_generate_object(ns, type, LAYOUT_RT_SURFACE, id)))
			return(NULL);
		if (fe_add_surface(surface)) {
			surface_destroy_m(surface);
			return(NULL);
		}
		return(SURFACE_S(surface)->root);
	}
	else {
		if (!(widget = (struct widget_s *) layout_generate_object(ns, type, LAYOUT_RT_WIDGET, id)))
			return(NULL);
		widget_control(parent, WCC_ADD_WIDGET, widget);
		return(widget);
	}
}

int fe_destroy_widget(void *widget)
{
	struct queue_node_s *cur;

	// TODO should there be some kind of lock that prevents destruction of key widgets?
	if (!widget)
		return(-1);
	else if (WIDGET_S(widget)->parent) {
		widget_control(WIDGET_S(widget)->parent, WCC_REMOVE_WIDGET, widget);
		destroy_widget(widget);
		return(0);
	}
	else {
		queue_foreach(surface_list, cur) {
			if (SURFACE_S(cur->ptr)->root == widget) {
				queue_delete_node(surface_list, cur);
				return(0);
			}
		}
	}
	return(-1);
}

void *fe_get_parent(void *widget)
{
	return(WIDGET_S(widget)->parent);
}

short fe_get_width(void *widget)
{
	widget_size_t size;

	if (widget_control(widget, WCC_GET_WINDOW, NULL, &size))
		return(-1);
	return(size.width);
}

short fe_get_height(void *widget)
{
	widget_size_t size;

	if (widget_control(widget, WCC_GET_WINDOW, NULL, &size))
		return(-1);
	return(size.height);
}

void *fe_find_widget(char *id)
{
	return(find_widget(id));
}

int fe_resize_widget(void *widget, int x, int y)
{
	// TODO fill in
	return(-1);
}


void *fe_current_widget(char *context, void *ref)
{
	struct widget_s *widget;
	struct queue_node_s *node;

	if (!ref) {
		if (!(node = queue_current_node(surface_list)))
			node = queue_set_current_node(surface_list, queue_first_node(surface_list));
		if (!node || !node->ptr || !(ref = SURFACE_S(node->ptr)->root))
			return(NULL);
	}

	if (widget_control(WIDGET_S(ref), WCC_CURRENT_WIDGET, &widget, context)) {
		if (strstr(WIDGET_S(ref)->type->name, context))
			return(ref);
		else
			return(NULL);
	}
	return(widget);
}

int fe_select_widget(char *context, void *ref, void *widget)
{
	struct queue_node_s *cur;

	if (!ref) {
		// TODO should you send a command to the surface to cause it to become active?
		queue_foreach(surface_list, cur) {
			if (!(widget_control(SURFACE_S(cur->ptr)->root, WCC_SELECT_WIDGET, context, widget)))
				return(0);
		}
		return(-1);
	}
	return(widget_control(WIDGET_S(ref), WCC_SELECT_WIDGET, context, widget));
}

void *fe_next_widget(char *context, void *ref)
{
	struct widget_s *widget;
	struct queue_node_s *node;

	if (!ref) {
		if (!(node = queue_set_current_node(surface_list, queue_next_node(queue_current_node(surface_list)))) || !node->ptr)
			return(NULL);
		return(SURFACE_S(node->ptr)->root);
	}
	if (widget_control(WIDGET_S(ref), WCC_NEXT_WIDGET, &widget, context))
		return(NULL);
	return(widget);
}

void *fe_previous_widget(char *context, void *ref)
{
	struct widget_s *widget;
	struct queue_node_s *node;

	if (!ref) {
		if (!(node = queue_set_current_node(surface_list, queue_previous_node(queue_current_node(surface_list)))) || !node->ptr)
			return(NULL);
		return(SURFACE_S(node->ptr)->root);
	}
	if (widget_control(WIDGET_S(ref), WCC_PREVIOUS_WIDGET, &widget, context))
		return(NULL);
	return(widget);
}

void *fe_first_widget(char *context, void *ref)
{
	struct widget_s *widget;
	struct queue_node_s *node;

	if (!ref) {
		if (!(node = queue_set_current_node(surface_list, queue_first_node(surface_list))) || !node->ptr)
			return(NULL);
		return(SURFACE_S(node->ptr)->root);
	}
	if (widget_control(WIDGET_S(ref), WCC_FIRST_WIDGET, &widget, context))
		return(NULL);
	return(widget);
}

void *fe_last_widget(char *context, void *ref)
{
	struct widget_s *widget;
	struct queue_node_s *node;

	if (!ref) {
		if (!(node = queue_set_current_node(surface_list, queue_last_node(surface_list))) || !node->ptr)
			return(NULL);
		return(SURFACE_S(node->ptr)->root);
	}
	if (widget_control(WIDGET_S(ref), WCC_LAST_WIDGET, &widget, context))
		return(NULL);
	return(widget);
}


int fe_print(void *widget, const char *str)
{
	return(widget_print_m(widget, str, -1));
}

char *fe_read(void *widget, char *buffer, int max)
{
	if (widget_read_m(widget, buffer, max))
		return(NULL);
	return(buffer);
}

void fe_clear(void *widget)
{
	widget_clear_m(widget);
}

void fe_move(void *widget, short x, short y)
{
	widget_control(WIDGET_S(widget), WCC_MODIFY_POSITION, x, y);
}

int fe_scroll(void *widget, int diff)
{
	return(widget_control(WIDGET_S(widget), WCC_SCROLL, diff));
}

void fe_quit(char *reason)
{
	emit_signal(NULL, "fe.quit", (void *) reason);
}

int fe_refresh(void)
{
	struct queue_node_s *cur;

	queue_foreach(surface_list, cur) {
		surface_control_m(cur->ptr, SCC_REFRESH, 0);
	}
	return(0);
}

/*** Local Functions ***/

// TODO this is here until you can decide on how layouts will be created
static int add_builtin_layouts(void)
{
	struct layout_s *layout;

	layout = make_layout("frame", make_layout_property("id", "frame", NULL), NULL, NULL);
	layout->next = make_layout("statusbar", make_layout_property("id", "statusbar", make_layout_property("text", FE_STATUSBAR, NULL)), NULL, NULL);
	layout->next->next = make_layout("input", make_layout_property("id", "input", NULL), NULL, NULL);

	add_layout("", "root", make_layout("terminal", NULL, make_layout("region", make_layout_property("id", "region", make_layout_property("width", "80", make_layout_property("height", "25", NULL))), layout, NULL), NULL));
	add_layout("", "text", make_layout("text", NULL, NULL, NULL));

	return(0);
}

static int fe_add_surface(struct surface_s *surface)
{
	struct queue_node_s *node;

	if (!(node = queue_create_node(0)))
		return(-1);
	queue_init_node(surface_list, node, surface);
	queue_append_node(surface_list, node);

	if (!queue_current_node(surface_list))
		queue_set_current_node(surface_list, node);
	signal_connect(surface, "purge_object", 10, (signal_t) fe_handle_purge_surface, NULL);
	return(0);
}

static int fe_destroy_surface(struct surface_s *surface)
{
	if (surface)
		surface_destroy_m(surface);
	return(0);
}

static int fe_handle_purge_surface(void *ptr, struct surface_s *surface)
{
	struct queue_node_s *cur;

	queue_foreach(surface_list, cur) {
		if (SURFACE_S(cur->ptr) == surface) {
			cur->ptr = NULL;
			queue_delete_node(surface_list, cur);
			return(0);
		}
	}
	return(0);
}


