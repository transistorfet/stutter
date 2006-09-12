/*
 * Header Name:		scroll.h
 * Version:		0.1
 * Description:		Scrolling Window Header
 */

#ifndef _COMMON_WIDGETS_SCROLL_H
#define _COMMON_WIDGETS_SCROLL_H

#include <stdarg.h>

#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "../utils/format.h"

struct scroll_entry_s {
	struct format_string_s *format;
	queue_node_v(scroll_entry_s) log;
};

struct scroll_s {
	struct widget_s widget;
	int cur_line;
	int max_lines;
	queue_list_v(scroll_entry_s) log;
};

extern struct widget_type_s scroll_type;

int scroll_init(struct scroll_s *);
int scroll_release(struct scroll_s *);
int scroll_refresh(struct scroll_s *);
int scroll_print(struct scroll_s *, const char *, int);
void scroll_clear(struct scroll_s *);
int scroll_read(struct scroll_s *, char *, int);
int scroll_control(struct scroll_s *, int, va_list);

#endif


