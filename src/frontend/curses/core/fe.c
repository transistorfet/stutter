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
#include <stutter/variable.h>
#include <stutter/lib/queue.h>
#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "net.h"

extern struct surface_s *terminal;

extern struct widget_type_s frame_type;
extern struct widget_type_s input_type;
extern struct widget_type_s scroll_type;
extern struct widget_type_s window_type;
extern struct widget_type_s statusbar_type;

struct widget_s *frame;
struct widget_s *input;
struct widget_s *statusbar;

static void fe_update_size(void);

int init_frontend(void)
{
	if (!(frame = (struct widget_s *) create_widget(&frame_type, "frame", NULL)))
		return(-1);
	widget_control(frame, WCC_SET_SURFACE, terminal);

	if (!(statusbar = (struct widget_s *) create_widget(&statusbar_type, "statusbar", NULL)))
		return(-1);
	widget_control(statusbar, WCC_SET_SURFACE, terminal);
	widget_print_m(statusbar, FE_STATUSBAR, -1);

	if (!(input = (struct widget_s *) create_widget(&input_type, "input", NULL)))
		return(-1);
	widget_control(input, WCC_SET_SURFACE, terminal);

	fe_update_size();
	return(0);
}

int release_frontend(void)
{
	if (input)
		destroy_widget((struct widget_s *) input);
	if (statusbar)
		destroy_widget((struct widget_s *) statusbar);
	if (frame)
		destroy_widget((struct widget_s *) frame);
	return(0);
}

void *fe_create_widget(char *ns, char *type, char *id, void *parent)
{
	struct window_s *window;

	if (strcmp(type, "window") || !(window = (struct window_s *) create_widget(&scroll_type, id, frame)))
		return(NULL);
	widget_control(frame, WCC_ADD_WIDGET, window);
	return(window);
}

int fe_destroy_widget(void *widget)
{
	if ((widget == statusbar) || (widget == input))
		return(-1);
	widget_control(frame, WCC_REMOVE_WIDGET, widget);
	return(0);
}

void *fe_get_parent(void *widget)
{
	return(((struct widget_s *) widget)->parent);
}

short fe_get_width(void *widget)
{
	// TODO fill in
	return(-1);
}

short fe_get_height(void *widget)
{
	// TODO fill in
	return(-1);
}

void *fe_find_widget(char *id)
{
	// TODO fill in
	return(NULL);
}

int fe_resize_widget(void *widget, int diffx, int diffy)
{
	// TODO fill in
	return(-1);
}


void *fe_current_widget(char *context, void *ref)
{
	struct widget_s *widget;

	if (!strcmp(context, "input"))
		return(input);
	else {
		widget_control(frame, WCC_CURRENT_WIDGET, &widget, context, ref);
		return(widget);
	}
}

int fe_select_widget(char *context, void *ref, void *widget)
{
	if (!strcmp(context, "input"))
		return(-1);
	return(widget_control(frame, WCC_SELECT_WIDGET, context, ref, widget));
}

void *fe_next_widget(char *context, void *ref)
{
	struct widget_s *widget;

	if (!strcmp(context, "input"))
		return(input);
	else {
		widget_control(frame, WCC_NEXT_WIDGET, &widget, context, ref);
		return(widget);
	}
	return(NULL);
}

void *fe_previous_widget(char *context, void *ref)
{
	struct widget_s *widget;

	if (!strcmp(context, "input"))
		return(input);
	else {
		widget_control(frame, WCC_PREVIOUS_WIDGET, &widget, context, ref);
		return(widget);
	}
	return(NULL);
}

void *fe_first_widget(char *context, void *ref)
{
	struct widget_s *widget;

	if (!strcmp(context, "input"))
		return(input);
	else {
		widget_control(frame, WCC_FIRST_WIDGET, &widget, context, ref);
		return(widget);
	}
	return(NULL);
}

void *fe_last_widget(char *context, void *ref)
{
	struct widget_s *widget;

	if (!strcmp(context, "input"))
		return(input);
	else {
		widget_control(frame, WCC_LAST_WIDGET, &widget, context, ref);
		return(widget);
	}
	return(NULL);
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
	widget_control((struct widget_s *) widget, WCC_MODIFY_POSITION, x, y);
}

int fe_scroll(void *widget, int diff)
{
	widget_control((struct widget_s *) widget, WCC_SCROLL, diff);
}

void fe_refresh(void)
{
	fe_update_size();
	widget_refresh_m(frame);
	widget_refresh_m(statusbar);
	widget_refresh_m(input);
}

void fe_quit(char *reason)
{
	emit_signal("fe.quit", NULL, (void *) reason);
}

/*** Local Functions ***/

static void fe_update_size(void)
{
	widget_control(frame, WCC_MODIFY_SIZE, surface_get_width_m(terminal), surface_get_height_m(terminal) - 2);
	widget_control(frame, WCC_MODIFY_POSITION, 0, 0);
	widget_control(statusbar, WCC_MODIFY_SIZE, surface_get_width_m(terminal), 1);
	widget_control(statusbar, WCC_MODIFY_POSITION, 0, surface_get_height_m(terminal) - 2);
	widget_control(input, WCC_MODIFY_SIZE, surface_get_width_m(terminal), 1);
	widget_control(input, WCC_MODIFY_POSITION, 0, surface_get_height_m(terminal) - 1);
}


