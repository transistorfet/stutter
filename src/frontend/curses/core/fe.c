/*
 * Module Name:		fe.c
 * Version:		0.1
 * Module Requirements:	type ; signal ; queue ; string ; variable ; net ; terminal
 * Description:		Curses Frontend
 */


#include <stdio.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>

extern struct surface_s *terminal;

extern struct widget_type_s text_type;
extern struct widget_type_s frame_type;
extern struct widget_type_s input_type;
extern struct widget_type_s window_type;
extern struct widget_type_s region_type;
extern struct widget_type_s statusbar_type;

struct widget_s *root;
struct widget_s *frame;
struct widget_s *input;
struct widget_s *statusbar;

int init_frontend(void)
{
	if (init_widget())
		return(-1);

	if (!(frame = (struct widget_s *) create_widget(&frame_type, "frame", NULL)))
		return(-1);

	if (!(statusbar = (struct widget_s *) create_widget(&statusbar_type, "statusbar", NULL)))
		return(-1);
	widget_print_m(statusbar, FE_STATUSBAR, -1);

	if (!(input = (struct widget_s *) create_widget(&input_type, "input", NULL)))
		return(-1);

	if (!(root = (struct widget_s *) create_widget(&region_type, "region", NULL)))
		return(-1);
	widget_control(root, WCC_SET_SURFACE, terminal);
	widget_control(root, WCC_SET_WINDOW, 0, 0, surface_get_width_m(terminal), surface_get_height_m(terminal));
	widget_control(root, WCC_ADD_WIDGET, frame);
	widget_control(root, WCC_ADD_WIDGET, statusbar);
	widget_control(root, WCC_ADD_WIDGET, input);
	return(0);
}

int release_frontend(void)
{
	destroy_widget(WIDGET_S(root));
	return(0);
}

void *fe_create_widget(char *ns, char *type, char *id, void *parent)
{
	struct window_s *window;

	if (!parent)
		return(NULL);
	if (strcmp(type, "text") || !(window = (struct window_s *) create_widget(&text_type, id, (struct widget_s *) parent)))
		return(NULL);
	widget_control(frame, WCC_ADD_WIDGET, window);
	return(window);
}

int fe_destroy_widget(void *widget)
{
	if ((widget == root) || (widget == frame) || (widget == statusbar) || (widget == input))
		return(-1);
	widget_control(frame, WCC_REMOVE_WIDGET, widget);
	destroy_widget(widget);
	return(0);
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
	// TODO should this really be absolute size and not difference in size?
	if (widget && !widget_control(widget, WCC_MODIFY_SIZE, NULL, x, y))
		return(0);
	// TODO how do you know what size to resize the surface to?  should we just let
	//	the root widget do that when it gets the MODIFY_SIZE command?
	return(-1);
}


void *fe_current_widget(char *context, void *ref)
{
	struct widget_s *widget;

	if (widget_control(ref ? WIDGET_S(ref) : root, WCC_CURRENT_WIDGET, &widget, context)) {
		if (strstr(root->type->name, context))
			return(root);
		else
			return(NULL);
	}
	return(widget);
}

int fe_select_widget(char *context, void *ref, void *widget)
{
	return(widget_control(ref ? WIDGET_S(ref) : root, WCC_SELECT_WIDGET, context, widget));
}

void *fe_next_widget(char *context, void *ref)
{
	struct widget_s *widget;

	if (widget_control(ref ? WIDGET_S(ref) : root, WCC_NEXT_WIDGET, &widget, context))
		return(NULL);
	return(widget);
}

void *fe_previous_widget(char *context, void *ref)
{
	struct widget_s *widget;


	if (widget_control(ref ? WIDGET_S(ref) : root, WCC_PREVIOUS_WIDGET, &widget, context))
		return(NULL);
	return(widget);
}

void *fe_first_widget(char *context, void *ref)
{
	struct widget_s *widget;

	if (widget_control(ref ? WIDGET_S(ref) : root, WCC_FIRST_WIDGET, &widget, context))
		return(NULL);
	return(widget);
}

void *fe_last_widget(char *context, void *ref)
{
	struct widget_s *widget;

	if (widget_control(ref ? WIDGET_S(ref) : root, WCC_LAST_WIDGET, &widget, context))
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
	widget_control(WIDGET_S(widget), WCC_SCROLL, diff);
}

void fe_refresh(void)
{
	widget_size_t size;

	widget_control(root, WCC_GET_WINDOW, NULL, &size);
	if ((surface_get_width_m(terminal) != size.width) || (surface_get_height_m(terminal) != size.height))
		widget_control(root, WCC_SET_WINDOW, 0, 0, surface_get_width_m(terminal), surface_get_height_m(terminal));
	widget_refresh_m(root);
}

void fe_quit(char *reason)
{
	emit_signal("fe.quit", NULL, (void *) reason);
}


