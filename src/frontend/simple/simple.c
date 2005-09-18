/*
 * Module Name:		simple.c
 * Version:		0.1
 * Module Requirements:	string ; screen ; keyboard
 * Description:		Simple Frontend
 */


#include CONFIG_H
#include <nit/net.h>
#include <nit/list.h>
#include <nit/queue.h>
#include <nit/string.h>
#include <nit/screen.h>
#include <nit/callback.h>
#include <nit/keyboard.h>
#include "simple.h"
#include "window.h"
#include "input.h"

int exit_flag = 1;
struct input_s *input;
struct list_s *key_list;
struct screen_s *screen;
struct queue_s *window_list;
struct callback_s *status_bar = NULL;

extern int main_idle(void);

static int refresh(char *, struct screen_s *);

int init_frontend(void)
{
	init_screen();

	if (!(screen = create_screen()))
		return(-1);
	screen_add_refresh(screen, create_callback(0, 0, NULL, (callback_t) refresh, NULL));
	if (!(window_list = create_queue(0, NULL, (destroy_t) destroy_window)))
		return(-1);
	if (!(input = create_input(0, 0)))
		return(-1);

	if (!(key_list = create_list(0, (compare_t) compare_callback_key, (destroy_t) destroy_callback)))
		return(-1);

	return(0);
}

int release_frontend(void)
{
	destroy_list(key_list);

	destroy_input(input);
	destroy_queue(window_list);
	destroy_screen(screen);

	release_screen();

	return(0);
}

void *fe_create_widget(char *type, void *parent)
{
	struct window_s *window;

	if (!(window = create_window(100)))
		return(NULL);
	queue_append(window_list, window);
	return(window);
}

int fe_destroy_widget(void *window)
{
	return(queue_delete(window_list, window));
}


short fe_get_width(void *widget)
{
	return(screen_width(screen));
}

short fe_get_height(void *widget)
{
	if (widget == status_bar)
		return(1);
	return(screen_height(screen) - 2);
}

int fe_print(void *widget, string_t str)
{
	if (widget == status_bar) {
		screen_set_attrib(screen, SC_INVERSE);
		screen_print(screen, str, 0);
		screen_set_attrib(screen, 0);
		destroy_string(str);
	}
	else
		window_print((struct window_s *) widget, str);
	return(0);
}

void fe_clear(void *widget)
{
	if (widget == status_bar) {
		screen_set_attrib(screen, SC_INVERSE);
		screen_clear(screen, 0, screen_height(screen) - 2, screen_width(screen), 1);
		screen_set_attrib(screen, 0);
	}
}

void fe_move(void *widget, short x, short y)
{
	if (widget == status_bar)
		screen_move(screen, x, screen_height(screen) - 2);
}

int fe_scroll(void *widget, int amount)
{
	if (widget != status_bar)
		return(window_scroll(widget, amount));
}

int fe_check_input(void)
{
	int ch;
	struct callback_s *callback;

	if (ch = keyboard_read_char()) {
		list_clear_current(key_list);
		if (callback = list_find(key_list, (void *) ch, 0)) {
			if (!execute_callback(callback, (void *) ch))
				return(0);
		}
		else
			input_default(input, ch);
	}
	return(0);
}

int fe_register_widget(char *name, struct callback_s *refresh)
{
	if (!strcmp(name, "irc:statusbar"))
		status_bar = refresh;
	return(0);
}

/**
 * Main Entry Point
 */
main(int argc, char **argv)
{
	if (init_frontend()) {
		printf("Failed to initialize frontend\n");
		return(0);
	}
	if (init_system()) {
		printf("Failed to initialize system\n");
		release_system();
		return(0);
	}

	while (exit_flag) {
		fe_refresh();
		// TODO replace main_idle with a set of alarms/timers set by init_system
	//	main_idle();
		net_wait(1);
		fe_check_input();
	}

	release_system();
	release_frontend();
}

/*** Local Functions ***/

static int refresh(char *env, struct screen_s *screen)
{
	struct window_s *current;

	if (!(current = queue_current(window_list)) && !(current = queue_first(window_list)))
		screen_clear(screen, 0, 0, screen_width(screen), screen_height(screen) - 2);
	else
		refresh_window(current, screen);
	execute_callback(status_bar, status_bar);
	refresh_input(input, screen);
	return(0);
}


