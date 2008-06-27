/*
 * Header Name:		frame.h
 * Description:		Frame Widget Header
 */

#ifndef _COMMON_WIDGETS_FRAME_H
#define _COMMON_WIDGETS_FRAME_H

#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/container.h>

struct fe_frame {
	struct fe_container container;
};

#define FE_FRAME(ptr)		( (struct fe_frame *) ptr )

extern struct fe_widget_type fe_frame_type;

int fe_frame_init(struct fe_frame *, const char *, va_list);
void fe_frame_release(struct fe_frame *);
int fe_frame_write(struct fe_frame *, const char *, int);
int fe_frame_read(struct fe_frame *, char *, int);
int fe_frame_refresh(struct fe_frame *, struct fe_surface *);
void fe_frame_clear(struct fe_frame *, struct fe_surface *);
int fe_frame_control(struct fe_frame *, int, va_list);

#endif

