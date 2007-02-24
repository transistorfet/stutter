/*
 * Header Name:		text.h
 * Version:		0.1
 * Description:		Text Window Header
 */

#ifndef _COMMON_WIDGETS_TEXT_H
#define _COMMON_WIDGETS_TEXT_H

#include <stdarg.h>

#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "../utils/format.h"

struct text_entry_s {
	char *str;
	queue_node_v(text_entry_s) log;
};

struct text_s {
	struct widget_s widget;
	int cur_line;
	int max_lines;
	queue_list_v(text_entry_s) log;
};

#define TEXT_S(widget)	((struct text_s *) widget)

extern struct widget_type_s text_type;

int text_init(struct text_s *, struct widget_attrib_s *);
int text_release(struct text_s *);
int text_refresh(struct text_s *);
int text_print(struct text_s *, const char *, int);
void text_clear(struct text_s *);
int text_read(struct text_s *, char *, int);
int text_control(struct text_s *, int, va_list);

#endif


