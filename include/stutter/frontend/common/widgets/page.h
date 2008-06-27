/*
 * Header Name:		page.h
 * Description:		Page Widget Header
 */

#ifndef _COMMON_WIDGETS_PAGE_H
#define _COMMON_WIDGETS_PAGE_H

#include <stutter/queue.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/frame.h>
#include <stutter/frontend/common/widgets/input.h>
#include <stutter/frontend/common/widgets/window.h>
#include <stutter/frontend/common/widgets/statusbar.h>

struct fe_page {
	struct fe_window window;
	struct fe_frame *frame;
	struct fe_statusbar *statusbar;
	struct fe_input *input;
};

#define FE_PAGE(ptr)		( (struct fe_page *) ptr )

extern struct fe_widget_type fe_page_type;

int fe_page_init(struct fe_page *, const char *, va_list);
void fe_page_release(struct fe_page *);
int fe_page_write(struct fe_page *, const char *, int);
int fe_page_read(struct fe_page *, char *, int);
int fe_page_refresh(struct fe_page *, struct fe_surface *);
void fe_page_clear(struct fe_page *, struct fe_surface *);
int fe_page_control(struct fe_page *, int, va_list);

#endif

