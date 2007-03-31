/*
 * Header Name:		input.h
 * Version:		0.1
 * Description:		Input Buffer Header
 */

#ifndef _COMMON_WIDGETS_INPUT_H
#define _COMMON_WIDGETS_INPUT_H

#include <stdarg.h>

#include <stutter/string.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "window.h"

struct input_s {
	struct window_s window;
	string_t context;
	struct widget_s *target;
	int i;
	int end;
	int max;
	char *buffer;
	struct queue_s *history;
};

#define INPUT_S(widget)	((struct input_s *) widget)

extern struct widget_type_s input_type;

int input_init(struct input_s *, struct property_s *);
int input_release(struct input_s *);
int input_refresh(struct input_s *);
int input_print(struct input_s *, const char *, int);
void input_clear(struct input_s *);
int input_read(struct input_s *, char *, int);
int input_control(struct input_s *, int, va_list);

#endif


