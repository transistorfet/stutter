/*
 * Header Name:		container.h
 * Version:		0.1
 * Description:		Generic Container Widget Header
 */

#ifndef _COMMON_WIDGETS_CONTAINER_H
#define _COMMON_WIDGETS_CONTAINER_H

#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "window.h"

struct container_s {
	struct window_s window;
	struct queue_s *widgets;
};

#define CONTAINER_S(widget)	((struct container_s *) widget)

extern struct widget_type_s container_type;

int container_init(struct container_s *);
int container_release(struct container_s *);
int container_refresh(struct container_s *);
int container_print(struct container_s *, const char *, int);
void container_clear(struct container_s *);
int container_read(struct container_s *, char *, int);
int container_control(struct container_s *, int, va_list);

#endif

