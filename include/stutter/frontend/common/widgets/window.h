/*
 * Header Name:		window.h
 * Description:		Window Manager Header
 */

#ifndef _COMMON_WIDGETS_WINDOW_H
#define _COMMON_WIDGETS_WINDOW_H

#include <stdarg.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>

struct fe_window {
	struct fe_widget widget;
	short x, y;
	short width, height;
};

#define FE_WINDOW(ptr)		( (struct fe_window *) ptr )

extern struct fe_widget_type fe_window_type;

int fe_window_init(struct fe_window *, const char *, va_list);
void fe_window_release(struct fe_window *);
int fe_window_write(struct fe_window *, const char *, int);
int fe_window_read(struct fe_window *, char *, int);
int fe_window_refresh(struct fe_window *, struct fe_surface *);
void fe_window_clear(struct fe_window *, struct fe_surface *);
int fe_window_control(struct fe_window *, int, va_list);

#endif


