/*
 * Module Name:		simple.c
 * Version:		0.1
 * Module Requirements:	type ; queue ; string ; variable ; net ; screen ; keyboard
 * Description:		Simple Frontend
 */


#include <stdlib.h>

#include CONFIG_H
#include <type.h>
#include <variable.h>
#include <lib/queue.h>
#include <lib/string.h>
#include <lib/globals.h>
#include "input.h"
#include "window.h"
#include "statusbar.h"
#include "screen.h"
#include "net.h"

int exit_flag = 1;
struct input_s *input;
struct queue_s *window_list;
struct statusbar_s *statusbar;
struct queue_node_s *current_window;

int init_frontend(void)
{
	init_screen();

	if (!(window_list = create_queue(0, (destroy_t) destroy_window)))
		return(-1);
	if (!(statusbar = create_statusbar(FE_STATUS_BAR_HEIGHT, FE_NAMESPACE, FE_STATUS)))
		return(-1);
	if (!(input = create_input(0, 0)))
		return(-1);
	current_window = NULL;
	return(0);
}

int release_frontend(void)
{
	destroy_input(input);
	destroy_statusbar(statusbar);
	destroy_queue(window_list);
	release_screen();

	return(0);
}

void *fe_create_widget(char *ns, char *type, char *id, void *parent)
{
	struct window_s *window;

	if (strcmp(type, "window") || !(window = create_window(100)))
		return(NULL);
	queue_append(window_list, window);
	return(window);
}

int fe_destroy_widget(void *widget)
{
	if ((widget == statusbar) || (widget == input))
		return(-1);
	if (current_window->ptr == widget)
		current_window = NULL;
	queue_delete(window_list, widget);
	return(0);
}

void *fe_get_parent(void *widget)
{
	return(NULL);
}

short fe_get_width(void *widget)
{
	return(screen_width());
}

short fe_get_height(void *widget)
{
	if (widget == statusbar)
		return(statusbar->height);
	else if (widget == input)
		return(1);
	return(screen_height() - 2);
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
	else if (current_window)
		return(current_window->ptr);
	return(NULL);
}

int fe_select_widget(char *context, void *ref, void *widget)
{
	if (!strcmp(context, "input"))
		return(-1);
	if (current_window = queue_find(window_list, widget))
		return(0);
	return(-1);
}

void *fe_next_widget(char *context, void *ref)
{
	if (!strcmp(context, "input"))
		return(input);
	if (current_window = queue_next_node(current_window))
		return((struct window_s *) current_window->ptr);
	return(NULL);
}

void *fe_previous_widget(char *context, void *ref)
{
	if (!strcmp(context, "input"))
		return(input);
	if (current_window = queue_previous_node(current_window))
		return((struct window_s *) current_window->ptr);
	return(NULL);
}

void *fe_first_widget(char *context, void *ref)
{
	if (!strcmp(context, "input"))
		return(input);
	if (current_window = queue_first_node(window_list))
		return((struct window_s *) current_window->ptr);
	return(NULL);
}

void *fe_last_widget(char *context, void *ref)
{
	if (!strcmp(context, "input"))
		return(input);
	if (current_window = queue_last_node(window_list))
		return((struct window_s *) current_window->ptr);
	return(NULL);
}


int fe_print(void *widget, string_t str)
{
	if (widget == input) {
		input_set_buffer(input, str);
		destroy_string(str);
	}
	else
		window_print((struct window_s *) widget, str);
	return(0);
}

char *fe_read(void *widget)
{
	if (widget == input)
		return(input_get_buffer(input));
	return(NULL);
}

void fe_clear(void *widget)
{
	if (widget == statusbar) {
		screen_set_attribs(SC_INVERSE, 0, 0);
		screen_clear(0, screen_height() - statusbar->height - 1, screen_width(), statusbar->height);
		screen_set_attribs(SC_NORMAL, 0, 0);
	}
	else if (widget == input)
		clear_input(input);
	else
		window_clear(widget);
}

void fe_move(void *widget, short x, short y)
{
	if (widget == statusbar)
		screen_move(x, screen_height() - statusbar->height - 1);
}

int fe_scroll(void *widget, int diff)
{
	if (widget != statusbar)
		return(window_scroll(widget, diff));
}

void fe_refresh(void *widget)
{
	if (!widget) {
		if (!current_window && !(current_window = queue_first_node(window_list)))
			screen_clear(0, 0, screen_width(), screen_height() - statusbar->height - 1);
		else
			refresh_window((struct window_s *) current_window->ptr);
		refresh_statusbar(statusbar);
		refresh_input(input);
	}
	else if (widget == input)
		refresh_input(input);
	else if (widget == statusbar)
		refresh_statusbar(statusbar);
	else
		refresh_window(widget);
	screen_refresh();
}

void fe_terminate(int status)
{
	exit_flag = 0;
}

/**
 * Main Entry Point
 */
main(int argc, char **argv)
{
	int ch;
	struct type_s *type;

	if (init_system()) {
		printf("Failed to initialize system\n");
		release_system();
		return(0);
	}

	if (type = find_type("format"))
		add_variable(type, FE_NAMESPACE, FE_STATUS, type->create("%s", FE_STATUS_BAR));

	if (init_frontend()) {
		printf("Failed to initialize frontend\n");
		release_system();
		return(0);
	}

	fe_refresh(NULL);
	load_modules();

	while (exit_flag) {
		fe_refresh(NULL);
		// TODO rename net_wait (can it be moved somewhere outside the net module?)
		net_wait(1);
		if ((ch = screen_read_char()) && (process_key(ch)))
			input_default(input, ch);
	}

	unload_modules();
	release_frontend();
	release_system();
	return(0);
}


