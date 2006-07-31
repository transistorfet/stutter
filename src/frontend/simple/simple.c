/*
 * Module Name:		simple.c
 * Version:		0.1
 * Module Requirements:	type ; signal ; queue ; string ; variable ; net ; terminal
 * Description:		Simple Frontend
 */


#include <stdio.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/variable.h>
#include <stutter/lib/queue.h>
#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>

#include "input.h"
#include "window.h"
#include "terminal.h"
#include "statusbar.h"
#include "net.h"

int exit_flag = 1;
struct input_s *input;
struct queue_s *window_list;
struct statusbar_s *statusbar;

int init_frontend(void)
{
	init_terminal();

	if (!(window_list = create_queue(0, (destroy_t) destroy_window)))
		return(-1);
	if (!(statusbar = create_statusbar(FE_STATUS_BAR_HEIGHT, FE_STATUS)))
		return(-1);
	if (!(input = create_input(0, 0)))
		return(-1);
	return(0);
}

int release_frontend(void)
{
	destroy_input(input);
	destroy_statusbar(statusbar);
	destroy_queue(window_list);
	release_terminal();

	return(0);
}

void *fe_create_widget(char *ns, char *type, char *id, void *parent)
{
	struct window_s *window;

	if (strcmp(type, "window") || !(window = create_window(FE_WINDOW_LOG_SIZE)))
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
	return(terminal_width());
}

short fe_get_height(void *widget)
{
	if (widget == statusbar)
		return(statusbar->height);
	else if (widget == input)
		return(1);
	return(terminal_height() - 2);
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
		terminal_set_attribs(TA_INVERSE, 0, 0);
		terminal_clear(0, terminal_height() - statusbar->height - 1, terminal_width(), statusbar->height);
		terminal_set_attribs(TA_NORMAL, 0, 0);
	}
	else if (widget == input)
		clear_input(input);
	else
		window_clear(widget);
}

void fe_move(void *widget, short x, short y)
{
	if (widget == statusbar)
		terminal_move(x, terminal_height() - statusbar->height - 1);
}

int fe_scroll(void *widget, int diff)
{
	if (widget != statusbar)
		return(window_scroll(widget, diff));
}

void fe_refresh(void *widget)
{
	struct window_s *window;

	if (!widget) {
		if (!queue_current_node(window_list) && !queue_first_node(window_list))
			terminal_clear(0, 0, terminal_width(), terminal_height() - statusbar->height - 1);
		else if (window = (struct window_s *) queue_current(window_list))
			refresh_window(window);
		refresh_statusbar(statusbar);
		refresh_input(input);
	}
	else if (widget == input)
		refresh_input(input);
	else if (widget == statusbar)
		refresh_statusbar(statusbar);
	else
		refresh_window(widget);
	terminal_refresh();
}

void fe_quit(char *reason)
{
	signal_emit("quit", NULL, (void *) reason);
	exit_flag = 0;
}

/**
 * Main Entry Point
 */
main(int argc, char **argv)
{
	int ch;
	struct type_s *type;
	struct variable_s *table;

	if (init_system()) {
		printf("Failed to initialize system\n");
		release_system();
		return(0);
	}

	add_signal("quit", 0);

	LOAD_MODULES();
	if (!(type = find_type("table")) || !(table = add_variable(NULL, type, "fe", 0, "")))
		return(-1);
	if (type = find_type("format"))
		add_variable(NULL, type, FE_STATUS, 0, "%s", FE_STATUS_BAR);
	if (type = find_type("string")) {
		add_variable(NULL, type, "fe.bold", 0, "%s", "\x02");
	}
	BIND_KEYS();

	if (init_frontend()) {
		printf("Failed to initialize frontend\n");
		release_system();
		return(0);
	}

	fe_refresh(NULL);
	#ifdef STUTTER_INIT
	STUTTER_INIT(argc, argv);
	#endif

	while (exit_flag) {
		fe_refresh(NULL);
		fe_net_wait(1);
		if ((ch = terminal_read_char()) && (process_key(ch)))
			input_default(input, ch);
	}

	RELEASE_MODULES();
	release_frontend();
	release_system();
	return(0);
}


