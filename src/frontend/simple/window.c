/*
 * Module Name:		window.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; string ; screen
 * Description:		Window Manager
 */

#include <string.h>

#include <nit/debug.h>
#include <nit/queue.h>
#include <nit/memory.h>
#include <nit/string.h>
#include <nit/screen.h>
#include "window.h"

static int window_wrap_string(char *, int);

/**
 * Allocate and initialize a window structure given the intialization values.
 */
struct window_s *create_window(int size)
{
	struct window_s *window;

	if (!(window = (struct window_s *) memory_alloc(sizeof(struct window_s))))
		return(NULL);
	memset(window, '\0', sizeof(struct window_s));

	if (!(window->log = create_queue(size, NULL, (destroy_t) destroy_string))) {
		destroy_window(window);
		return(NULL);
	}

	return(window);
}

/**
 * Free all the resources used by the given window structure.
 */
int destroy_window(struct window_s *window)
{
	if (window->log)
		destroy_queue(window->log);
	memory_free(window);
	return(0);
 }

/**
 * Redraw the given window on the screen if it is not hidden.
 */
int refresh_window(struct window_s *window, struct screen_s *screen)
{
	char *str;
	int i, j, lines = 0;
	int breaks[WINDOW_MAX_WRAP];

	lines = screen_height(screen) - 3;
	screen_clear(screen, 0, 0, screen_width(screen), screen_height(screen) - 2);
	screen_move(screen, 0, lines);

	queue_clear_current(window->log);
	for (j = 0;j < window->cur_line;j++) {
		if (!queue_next(window->log))
			return(0);
	}
	while (lines >= 0) {
		if (!(str = queue_next(window->log)))
			break;

		j = 0;
		for (i = 0;i < WINDOW_MAX_WRAP;i++) {
			breaks[i] = window_wrap_string(&str[j], screen_width(screen));
			if (breaks[i] == 0)
				break;
			j += breaks[i];
		}

		for (;i >= 0;i--) {
			screen_move(screen, 0, lines);
			screen_print(screen, &str[j], breaks[i]);
			lines--;
			j -= breaks[i - 1];
		}
	}
	return(0);
}

/**
 * Add the given message to the log of the given window.
 */
int window_print(struct window_s *window, string_t str)
{
	if (!window || !str)
		return(-1);
	/** If the window is not at the bottom then make sure the screen doesn't move */
	if (window->cur_line && (window->cur_line < queue_max(window->log)))
		window->cur_line++;
	queue_prepend(window->log, str);
	return(0);
}

/**
 * Flush the buffer of stored lines from the given window and return
 * 0 on success or -1 on failure.
 */
int window_clear(struct window_s *window)
{
	struct queue_s *log;

	if (!window || !(log = create_queue(queue_max(window->log), NULL, (destroy_t) destroy_string)))
		return(-1);
	destroy_queue(window->log);
	window->log = log;
	return(0);
}

/**
 * Scroll the given window by the given amount.  A positive number will
 * scroll up and a negative number will scroll down.  If the end of the
 * window has been reached, -1 will be returned, otherwise 0.
 */
int window_scroll(struct window_s *window, short scroll)
{
	window->cur_line += scroll;
	if (window->cur_line < 0) {
		window->cur_line = 0;
		return(-1);
	}
	else if (window->cur_line > queue_size(window->log)) {
		window->cur_line = queue_size(window->log);
		return(-1);
	}
	return(0);
}

/*** Local Functions ***/

/**
 * Determine the index into the given string at which to break the string
 * in two given the maximum length of a each line.  If the line does not
 * need to be broken up, 0 is returned.
 */
static int window_wrap_string(char *str, int limit)
{
	int i;

	if (strlen(str) <= limit)
		return(0);
	i = limit;
	while ((i > 0) && (str[i] != 0x20))
		i--;
	if (i == 0)
		return(limit);
	return(i);
}



