/*
 * Module Name:		window.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; surface
 * Description:		Window Manager
 */

#include <string.h>

#include <stutter/lib/queue.h>
#include <stutter/lib/memory.h>
#include <stutter/frontend/surface.h>
#include "window.h"

static int window_wrap_string(char *, int);

/**
 * Allocate and initialize a window structure given the intialization values.
 */
struct window_s *create_window(struct surface_s *surface, int size)
{
	struct window_s *window;

	if (!(window = (struct window_s *) memory_alloc(sizeof(struct window_s))))
		return(NULL);
	memset(window, '\0', sizeof(struct window_s));
	window->max_lines = size;
	window->surface = surface;
	queue_init_v(window->log);
	return(window);
}

/**
 * Free all the resources used by the given window structure.
 */
int destroy_window(struct window_s *window)
{
	queue_destroy_v(window->log, log,
		memory_free(cur);
	);
	memory_free(window);
	return(0);
 }

/**
 * Redraw the given window on the terminal if it is not hidden.
 */
int refresh_window(struct window_s *window)
{
	char *str;
	short width, height;
	int i, j, lines = 0;
	struct window_entry_s *cur;
	int breaks[WINDOW_MAX_WRAP];

	width = surface_get_width_m(window->surface);
	height = surface_get_height_m(window->surface);

	// TODO how do you get the statusbar height?
	lines = height - 3;
	surface_clear_m(window->surface, 0, 0, width, height - 2);
	surface_move_m(window->surface, 0, lines);

	if (!(cur = queue_first_v(window->log)))
		return(0);
	for (j = 0;j < window->cur_line;j++) {
		if (!(cur = queue_next_v(cur, log)))
			return(0);
	}
	while (lines >= 0) {
		str = cur->line;
		j = 0;
		for (i = 0;i < WINDOW_MAX_WRAP;i++) {
			breaks[i] = window_wrap_string(&str[j], width);
			if (breaks[i] == -1)
				break;
			j += breaks[i];
		}

		for (;i >= 0;i--) {
			surface_move_m(window->surface, 0, lines);
			surface_print_m(window->surface, &str[j], breaks[i]);
			lines--;
			j -= breaks[i - 1];
		}
		if (!(cur = queue_next_v(cur, log)))
			break;
	}
	return(0);
}

/**
 * Add the given message to the log of the given window.
 */
int window_print(struct window_s *window, const char *str)
{
	struct window_entry_s *node;

	if (!window || !str)
		return(-1);
	/** If the window is not at the bottom then make sure the screen doesn't move */
	if (window->cur_line && (window->cur_line < window->max_lines))
		window->cur_line++;

	// TODO parse for new format using the common format parser
	if (!(node = (struct window_entry_s *) memory_alloc(sizeof(struct window_entry_s) + strlen(str) + 1)))
		return(-1);
	node->line = (char *) (((size_t) node) + sizeof(struct window_entry_s));
	strcpy(node->line, str);
	queue_prepend_node_v(window->log, node, log);

	return(0);
}

/**
 * Flush the buffer of stored lines from the given window and return
 * 0 on success or -1 on failure.
 */
int window_clear(struct window_s *window)
{
	struct queue_s *log;

	if (!window)
		return(-1);
	queue_destroy_v(window->log, log,
		memory_free(cur);
	);
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
	else if (window->cur_line > queue_size_v(window->log)) {
		window->cur_line = queue_size_v(window->log);
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
		return(-1);
	i = limit;
	while ((i > 0) && (str[i] != 0x20))
		i--;
	if (i == 0)
		return(limit);
	return(i);
}



