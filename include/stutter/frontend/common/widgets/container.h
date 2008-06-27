/*
 * Header Name:		container.h
 * Description:		Generic Container Widget Header
 */

#ifndef _COMMON_WIDGETS_CONTAINER_H
#define _COMMON_WIDGETS_CONTAINER_H

#include <stutter/queue.h>
#include <stutter/object.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/window.h>

struct fe_container {
	struct fe_window window;
	struct queue_s *widgets;
};

#define FE_CONTAINER(ptr)		( (struct fe_container *) ptr )

extern struct fe_widget_type fe_container_type;

int fe_container_init(struct fe_container *, const char *, va_list);
void fe_container_release(struct fe_container *);
int fe_container_write(struct fe_container *, const char *, int);
int fe_container_read(struct fe_container *, char *, int);
int fe_container_refresh(struct fe_container *, struct fe_surface *);
void fe_container_clear(struct fe_container *, struct fe_surface *);
int fe_container_control(struct fe_container *, int, va_list);

#endif

