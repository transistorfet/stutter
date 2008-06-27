/*
 * Header Name:		region.h
 * Description:		Region Widget Header
 */

#ifndef _COMMON_WIDGETS_REGION_H
#define _COMMON_WIDGETS_REGION_H

#include <stutter/queue.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/container.h>

struct fe_region {
	struct fe_container container;
	struct fe_widget *focus;
};

#define FE_REGION(ptr)		( (struct fe_region *) ptr )

extern struct fe_widget_type fe_region_type;

int fe_region_init(struct fe_region *, const char *, va_list);
void fe_region_release(struct fe_region *);
int fe_region_write(struct fe_region *, const char *, int);
int fe_region_read(struct fe_region *, char *, int);
int fe_region_refresh(struct fe_region *, struct fe_surface *);
void fe_region_clear(struct fe_region *, struct fe_surface *);
int fe_region_control(struct fe_region *, int, va_list);

#endif

