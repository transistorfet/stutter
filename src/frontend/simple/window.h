/*
 * Header Name:		window.h
 * Version:		0.1
 * Description:		Window Manager Header
 */

#ifndef _SIMPLE_WINDOW_H
#define _SIMPLE_WINDOW_H

#include <nit/queue.h>
#include <nit/string.h>
#include <nit/screen.h>

#ifndef WINDOW_MAX_WRAP
#define WINDOW_MAX_WRAP		20
#endif

struct window_s {
	short bitflags;
	short cur_line;
	struct queue_s *log;
};

struct window_s *create_window(int);
int destroy_window(struct window_s *);
int refresh_window(struct window_s *, struct screen_s *);

int window_print(struct window_s *, string_t);
int window_clear(struct window_s *);
int window_scroll(struct window_s *, short);

#endif


