/*
 * Header Name:		window.h
 * Version:		0.1
 * Description:		Window Manager Header
 */

#ifndef _SIMPLE_WINDOW_H
#define _SIMPLE_WINDOW_H

#include <stutter/lib/queue.h>
#include <stutter/lib/string.h>

#ifndef WINDOW_MAX_WRAP
#define WINDOW_MAX_WRAP		20
#endif

struct window_entry_s {
	string_t line;
	queue_node_v(window_entry_s) log;
};

struct window_s {
	short bitflags;
	short cur_line;
	int max_lines;
	queue_list_v(window_entry_s) log;
};

struct window_s *create_window(int);
int destroy_window(struct window_s *);
int refresh_window(struct window_s *);

int window_print(struct window_s *, string_t);
int window_clear(struct window_s *);
int window_scroll(struct window_s *, short);

#endif


