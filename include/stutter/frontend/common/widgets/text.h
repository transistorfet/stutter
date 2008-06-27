/*
 * Header Name:		text.h
 * Description:		Text Window Header
 */

#ifndef _COMMON_WIDGETS_TEXT_H
#define _COMMON_WIDGETS_TEXT_H

#include <stdarg.h>

#include <stutter/queue.h>
#include <stutter/frontend/common/format.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>

struct fe_text {
	struct fe_widget widget;
	int cur_line;
	int max_lines;
	struct queue_s *log;
};

#define FE_TEXT(ptr)		( (struct fe_text *) ptr )

extern struct fe_widget_type fe_text_type;

int fe_text_init(struct fe_text *, const char *, va_list);
void fe_text_release(struct fe_text *);
int fe_text_write(struct fe_text *, const char *, int);
int fe_text_read(struct fe_text *, char *, int);
int fe_text_refresh(struct fe_text *, struct fe_surface *);
void fe_text_clear(struct fe_text *, struct fe_surface *);
int fe_text_control(struct fe_text *, int, va_list);

#endif


