/*
 * Header Name:		statusbar.h
 * Description:		Status Bar Widget Header
 */

#ifndef _COMMON_WIDGETS_STATUSBAR_H
#define _COMMON_WIDGETS_STATUSBAR_H

#include <stdarg.h>

#include <stutter/string.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/window.h>

struct fe_statusbar {
	struct fe_window window;
	string_t text;
};

#define FE_STATUSBAR(ptr)		( (struct fe_statusbar *) ptr )

extern struct fe_widget_type fe_statusbar_type;

int fe_statusbar_init(struct fe_statusbar *, const char *, va_list);
void fe_statusbar_release(struct fe_statusbar *);
int fe_statusbar_write(struct fe_statusbar *, const char *, int);
int fe_statusbar_read(struct fe_statusbar *, char *, int);
int fe_statusbar_refresh(struct fe_statusbar *, struct fe_surface *);
void fe_statusbar_clear(struct fe_statusbar *, struct fe_surface *);
int fe_statusbar_control(struct fe_statusbar *, int, va_list);

#endif


