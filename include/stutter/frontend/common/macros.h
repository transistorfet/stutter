/*
 * Module Name:		macros.h
 * Description:		Common Frontend Macros
 */

#ifndef STUTTER_FRONTEND_COMMON_MACROS_H
#define STUTTER_FRONTEND_COMMON_MACROS_H

#include <stdio.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/globals.h>
#include <stutter/frontend/common.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/layout.h>
#include <stutter/frontend/common/surface.h>

/*

static inline void *fe_create_widget(char *ns, char *type, char *id, void *parent)
{
	struct fe_widget *widget;
	struct fe_surface *surface;

	if (!parent) {
		if (!(surface = FE_SURFACE(layout_generate_object(ns, type, LAYOUT_RT_SURFACE, id))))
			return(NULL);
		return(FE_SURFACE(surface)->root);
	}
	else {
		if (!(widget = FE_WIDGET(layout_generate_object(ns, type, LAYOUT_RT_WIDGET, id))))
			return(NULL);
		fe_widget_control(parent, WCC_ADD_WIDGET, widget);
		return(widget);
	}
}


static inline int fe_common_api_destroy_widget(void *widget)
{
	struct surface_s *surface;

	// TODO should there be some kind of lock that prevents destruction of key widgets?
	if (!widget)
		return(-1);
	else if (WIDGET_S(widget)->parent) {
		widget_control(WIDGET_S(widget)->parent, WCC_REMOVE_WIDGET, widget);
		destroy_widget(widget);
		return(0);
	}
	else {
		if (widget_control(WIDGET_S(widget), WCC_GET_SURFACE, &surface) || !surface)
			return(-1);
		destroy_surface(surface);
	}
	return(-1);
}

static inline void *fe_common_api_get_parent(void *widget)
{
	return(WIDGET_S(widget)->parent);
}

static inline short fe_common_api_get_width(void *widget)
{
	widget_size_t size;

	if (widget_control(widget, WCC_GET_WINDOW, NULL, &size))
		return(-1);
	return(size.width);
}

static inline short fe_common_api_get_height(void *widget)
{
	widget_size_t size;

	if (widget_control(widget, WCC_GET_WINDOW, NULL, &size))
		return(-1);
	return(size.height);
}

static inline void *fe_common_api_find_widget(char *id)
{
	return(find_widget(id));
}

static inline int fe_common_api_resize_widget(void *widget, int x, int y)
{
	// TODO should this really be absolute size and not difference in size?
	if (widget && !widget_control(widget, WCC_MODIFY_SIZE, NULL, x, y))
		return(0);
	// TODO how do you know what size to resize the surface to?  should we just let
	//	the root widget do that when it gets the MODIFY_SIZE command?
	return(-1);
}


static inline void *fe_common_api_get_focus(char *type)
{
	struct widget_s *widget;
	struct surface_s *surface;

	if (!(surface = surface_get_current()) || !surface->root)
		return(NULL);

	if (widget_control(surface->root, WCC_GET_FOCUS, &widget))
		return(NULL);
	if (!type || strstr(WIDGET_S(widget)->type->name, type))
		return(widget);
	return(NULL);
}

static inline int fe_common_api_set_focus(void *widget)
{
	if (!widget)
		return(-1);
	return(widget_control(widget, WCC_SET_FOCUS, widget));
}

static inline void *fe_common_api_get_target(void *widget, char *type)
{
	struct widget_s *target;

	if (!widget && !(widget = fe_common_api_get_focus(NULL)))
		return(NULL);
	widget_control(WIDGET_S(widget), WCC_GET_TARGET, &target);
	if (!type)
		return(target);
	while (target) {
		if (strstr(target->type->name, type))
			return(target);
		if (widget_control(WIDGET_S(target), WCC_CURRENT_WIDGET, &widget) || !widget)
			return(NULL);
		target = widget;
	}
	return(NULL);
}

static inline int fe_common_api_set_target(void *widget)
{
	struct widget_s *parent, *child;

	child = WIDGET_S(widget);
	while (1) {
		if (!(parent = child->parent))
			break;
		if (widget_control(parent, WCC_SHOW_WIDGET, child))
			return(-1);
		child = parent;
	}
	if ((parent = fe_common_api_get_focus(NULL)))
		return(widget_control(parent, WCC_SET_TARGET, widget));
	return(0);
}

static inline int fe_common_api_show_widget( void *widget)
{
	struct widget_s *parent, *child;

	child = WIDGET_S(widget);
	while (1) {
		if (!(parent = child->parent))
			break;
		if (widget_control(parent, WCC_SHOW_WIDGET, child))
			return(-1);
		child = parent;
	}
	return(0);
}


static inline int fe_common_api_print(void *widget, const char *str)
{
	return(widget_print_m(widget, str, -1));
}

static inline int fe_common_api_read(void *widget, char *buffer, int max)
{
	return(widget_read_m(widget, buffer, max));
}

static inline void fe_common_api_clear(void *widget)
{
	widget_clear_m(widget);
}

static inline void fe_common_api_move(void *widget, short x, short y)
{
	widget_control(WIDGET_S(widget), WCC_MODIFY_POSITION, x, y);
}

static inline int fe_common_api_scroll(void *widget, int diff)
{
	return(widget_control(WIDGET_S(widget), WCC_SCROLL, diff));
}

static inline void fe_common_api_quit(char *reason)
{
	emit_signal(NULL, "fe.quit", (void *) reason);
}

static inline int fe_common_api_refresh(void)
{
	refresh_surface(NULL);
	return(0);
}
*/

#endif


