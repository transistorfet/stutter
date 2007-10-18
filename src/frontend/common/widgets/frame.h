/*
 * Header Name:		frame.h
 * Description:		Frame Widget Header
 */

#ifndef _COMMON_WIDGETS_FRAME_H
#define _COMMON_WIDGETS_FRAME_H

#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "container.h"

struct frame_s {
	struct container_s container;
};

#define FRAME_S(widget)	((struct frame_s *) widget)

extern struct widget_type_s frame_type;

int frame_init(struct frame_s *, struct property_s *);
int frame_release(struct frame_s *);
int frame_refresh(struct frame_s *);
int frame_print(struct frame_s *, const char *, int);
void frame_clear(struct frame_s *);
int frame_read(struct frame_s *, char *, int);
int frame_control(struct frame_s *, int, va_list);

#endif

