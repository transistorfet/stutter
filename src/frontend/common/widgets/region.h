/*
 * Header Name:		region.h
 * Version:		0.1
 * Description:		Frame Widget Header
 */

#ifndef _COMMON_WIDGETS_REGION_H
#define _COMMON_WIDGETS_REGION_H

#include <stutter/queue.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "container.h"

struct region_s {
	struct container_s container;
};

#define REGION_S(widget)	((struct region_s *) widget)

extern struct widget_type_s region_type;

int region_init(struct region_s *, struct property_s *);
int region_release(struct region_s *);
int region_refresh(struct region_s *);
int region_print(struct region_s *, const char *, int);
void region_clear(struct region_s *);
int region_read(struct region_s *, char *, int);
int region_control(struct region_s *, int, va_list);

#endif

