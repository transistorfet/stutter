/*
 * Module Name:		simple.c
 * Version:		0.1
 * Module Requirements:	type ; queue ; stringt ; variable ; net ; screen ; keyboard
 * Description:		Simple Frontend
 */


#include <stdlib.h>

#include CONFIG_H
#include <type.h>
#include <queue.h>
#include <globals.h>
#include <stringt.h>
#include <variable.h>
#include "input.h"
#include "window.h"
#include "statusbar.h"
#include "../common/unix/net.h"
#include "../common/curses/screen.h"
#include "../common/keyboard.h"

int exit_flag = 1;
struct input_s *input;
struct screen_s *screen;
struct queue_s *window_list;
struct statusbar_s *status_bar;
struct queue_node_s *current_window;

extern int main_idle(void);

static int check_input(void);
static int refresh(char *, struct screen_s *);

int init_frontend(void)
{
	init_screen();

	if (!(screen = create_screen()))
		return(-1);
	screen_add_refresh(screen, (callback_t) refresh);
	if (!(window_list = create_queue(0, (destroy_t) destroy_window)))
		return(-1);
	if (!(status_bar = create_statusbar(FE_STATUS_BAR_HEIGHT, FE_NAMESPACE, FE_STATUS)))
		return(-1);
	if (!(input = create_input(0, 0)))
		return(-1);
	current_window = NULL;
	return(0);
}

int release_frontend(void)
{
	destroy_input(input);
	destroy_statusbar(status_bar);
	destroy_queue(window_list);
	destroy_screen(screen);

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
	if ((widget == status_bar) || (widget == input))
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
	return(screen_width(screen));
}

short fe_get_height(void *widget)
{
	if (widget == status_bar)
		return(status_bar->height);
	else if (widget == input)
		return(1);
	return(screen_height(screen) - 2);
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
	return(current_window);
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
	if (widget == status_bar) {
		screen_set_attrib(screen, SC_INVERSE);
		screen_clear(screen, 0, screen_height(screen) - status_bar->height - 1, screen_width(screen), status_bar->height);
		screen_set_attrib(screen, 0);
	}
	else if (widget == input)
		clear_input(input);
	else
		window_clear(widget);
}

void fe_move(void *widget, short x, short y)
{
	if (widget == status_bar)
		screen_move(screen, x, screen_height(screen) - status_bar->height - 1);
}

int fe_scroll(void *widget, int diff)
{
	if (widget != status_bar)
		return(window_scroll(widget, diff));
}

void fe_refresh(void *widget)
{
	if (!widget)
		refresh_screen(screen);
	else if (widget == input)
		refresh_input(input, screen);
	else if (widget == status_bar)
		refresh_statusbar(status_bar, screen);
	else
		refresh_window(widget, screen);
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
		return(0);
	}

	fe_refresh(NULL);
	load_modules();

	while (exit_flag) {
		fe_refresh(NULL);
		// TODO rename net_wait (can it be moved somewhere outside the net module?)
		net_wait(1);
		check_input();
	}

	unload_modules();
	release_frontend();
	release_system();
	return(0);
}

/*** Local Functions ***/


static int check_input(void)
{
	int ch;

	if ((ch = keyboard_read_char()) && (process_key(ch)))
		input_default(input, ch);
	return(0);
}

static int refresh(char *env, struct screen_s *screen)
{
	if (!current_window && !(current_window = queue_first_node(window_list)))
		screen_clear(screen, 0, 0, screen_width(screen), screen_height(screen) - status_bar->height - 1);
	else
		refresh_window((struct window_s *) current_window->ptr, screen);
	refresh_statusbar(status_bar, screen);
	refresh_input(input, screen);
	return(0);
}


