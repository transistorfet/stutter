/*
 * Module Name:		fe.c
 * Version:		0.1
 * Module Requirements:	type ; signal ; queue ; string ; variable ; net ; terminal
 * Description:		Windows Frontend
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
#include <stutter/frontend/surface.h>
#include "input.h"
#include "window.h"
#include "statusbar.h"
#include "net.h"

extern struct surface_s *terminal;

struct input_s *input;
struct queue_s *window_list;
struct statusbar_s *statusbar;

int init_frontend(void)
{
	if (!(window_list = create_queue(0, (destroy_t) destroy_window)))
		return(-1);
	if (!(statusbar = create_statusbar(terminal, FE_STATUS_BAR_HEIGHT, FE_STATUS)))
		return(-1);
	if (!(input = create_input(terminal, 0, 0)))
		return(-1);
	return(0);
}

int release_frontend(void)
{
	if (input)
		destroy_input(input);
	if (statusbar)
		destroy_statusbar(statusbar);
	if (window_list)
		destroy_queue(window_list);
	return(0);
}

void *fe_create_widget(char *ns, char *type, char *id, void *parent)
{
	struct window_s *window;

	if (strcmp(type, "window") || !(window = create_window(terminal, FE_WINDOW_LOG_SIZE)))
		return(NULL);
	queue_append(window_list, window);
	return(window);
}

int fe_destroy_widget(void *widget)
{
	if ((widget == statusbar) || (widget == input))
		return(-1);
	queue_delete(window_list, widget);
	return(0);
}

void *fe_get_parent(void *widget)
{
	return(NULL);
}

short fe_get_width(void *widget)
{
	return(surface_get_width_m(terminal));
}

short fe_get_height(void *widget)
{
	if (widget == statusbar)
		return(statusbar->height);
	else if (widget == input)
		return(1);
	return(surface_get_height_m(terminal) - 2);
}

void *fe_find_widget(char *id)
{
	return(NULL);
}

int fe_resize_widget(void *widget, int diffx, int diffy)
{
	return(-1);
}


void *fe_current_widget(char *context, void *ref)
{
	if (!strcmp(context, "input"))
		return(input);
	else
		return(queue_current(window_list));
	return(NULL);
}

int fe_select_widget(char *context, void *ref, void *widget)
{
	if (!strcmp(context, "input"))
		return(-1);
	if (window_list->current = queue_find(window_list, widget))
		return(0);
	return(-1);
}

void *fe_next_widget(char *context, void *ref)
{
	struct window_s *window;

	if (!strcmp(context, "input"))
		return(input);
	if (window = (struct window_s *) queue_next(window_list))
		return(window);
	return(NULL);
}

void *fe_previous_widget(char *context, void *ref)
{
	struct window_s *window;

	if (!strcmp(context, "input"))
		return(input);
	if (window = (struct window_s *) queue_previous(window_list))
		return(window);
	return(NULL);
}

void *fe_first_widget(char *context, void *ref)
{
	struct window_s *window;

	if (!strcmp(context, "input"))
		return(input);
	if (window = (struct window_s *) queue_first(window_list))
		return(window);
	return(NULL);
}

void *fe_last_widget(char *context, void *ref)
{
	struct window_s *window;

	if (!strcmp(context, "input"))
		return(input);
	if (window = (struct window_s *) queue_last(window_list))
		return(window);
	return(NULL);
}


int fe_print(void *widget, const char *str)
{
	if (widget == input)
		input_set_buffer(input, str);
	else
		window_print((struct window_s *) widget, str);
	return(0);
}

char *fe_read(void *widget, char *buffer, int max)
{
	if (widget == input)
		return(input_get_buffer(input, buffer, max));
	return(NULL);
}

void fe_clear(void *widget)
{
	if (widget == statusbar) {
		surface_control_m(terminal, SCC_MODIFY_ATTRIB, SA_INVERSE, -1, -1);
		surface_clear_m(terminal, 0, surface_get_height_m(terminal) - statusbar->height - 1, surface_get_width_m(terminal), statusbar->height);
		surface_control_m(terminal, SCC_MODIFY_ATTRIB, SA_NORMAL, -1, -1);
	}
	else if (widget == input)
		clear_input(input);
	else
		window_clear(widget);
}

void fe_move(void *widget, short x, short y)
{
	if (widget == statusbar)
		surface_move_m(terminal, x, surface_get_height_m(terminal) - statusbar->height - 1);
}

int fe_scroll(void *widget, int diff)
{
	if (widget != statusbar)
		return(window_scroll(widget, diff));
}

void fe_refresh(void)
{
	struct window_s *window;

	if (!queue_current_node(window_list) && !queue_first_node(window_list))
		surface_clear_m(terminal, 0, 0, surface_get_width_m(terminal), surface_get_height_m(terminal) - statusbar->height - 1);
	else if (window = (struct window_s *) queue_current(window_list))
		refresh_window(window);
	refresh_statusbar(statusbar);
	refresh_input(input);
}

void fe_quit(char *reason)
{
	emit_signal("quit", NULL, (void *) reason);
}


