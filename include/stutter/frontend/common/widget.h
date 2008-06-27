/*
 * Header Name:		widget.h
 * Description:		Frontend Widget Header
 */

#ifndef _STUTTER_FRONTEND_WIDGET_H
#define _STUTTER_FRONTEND_WIDGET_H

#include <stdarg.h>
#include <string.h>

#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/frontend/common/layout.h>
#include <stutter/frontend/common/surface.h>

#define WT_DRAWING		0x0004
#define WT_CONTAINER		0x1000		/** A widget that has children widgets */
#define WT_FRAME		0x1001		/** A container in which only one child is visible */
#define WT_REGION		0x1002		/** A container in which all children are always visible */

#define WBF_NEEDS_REFRESH	0x0001
#define WBF_FORCE_REFRESH	0x0002

#define WCC_SET_PARENT		0x0001
#define WCC_SHOW_WIDGET		0x0002

#define WCC_GET_FOCUS		0x0003
#define WCC_SET_FOCUS		0x0004
#define WCC_GET_TARGET		0x0005
#define WCC_SET_TARGET		0x0006

#define WCC_GET_WINDOW		0x0010
#define WCC_SET_WINDOW		0x0011
#define WCC_MODIFY_SIZE		0x0012
#define WCC_MODIFY_POSITION	0x0013
#define WCC_GET_MIN_MAX_SIZE	0x0014

#define WCC_ADD_WIDGET		0x0020
#define WCC_INSERT_WIDGET	0x0021
#define WCC_REMOVE_WIDGET	0x0022
#define WCC_CURRENT_WIDGET	0x0023
#define WCC_SELECT_WIDGET	0x0024
#define WCC_NEXT_WIDGET		0x0025
#define WCC_PREVIOUS_WIDGET	0x0026
#define WCC_FIRST_WIDGET	0x0027
#define WCC_LAST_WIDGET		0x0028

#define WCC_SCROLL		0x0040
#define WCC_INSERT_CHAR		0x0041
#define WCC_PROCESS_CHAR	0x0042

typedef struct fe_widget_size {
	short width;
	short height;
} widget_size_t;

typedef struct fe_widget_pos {
	short x;
	short y;
} widget_pos_t;

struct fe_widget;
struct fe_surface;

#define FE_WIDGET(ptr)			( (struct fe_widget *) (ptr) )
#define FE_WIDGET_TYPE(ptr)		( (struct fe_widget_type *) (ptr) )
#define FE_WIDGET_GET_TYPE(ptr)		( FE_WIDGET_TYPE(OBJECT_S(ptr)->type) )

#define FE_WIDGET_WRITE(widget, str, len)							\
	( (FE_WIDGET_GET_TYPE(widget)->write)(FE_WIDGET(widget), str, len) )
#define FE_WIDGET_READ(widget, buffer, max)							\
	( (FE_WIDGET_GET_TYPE(widget)->read)(FE_WIDGET(widget), buffer, max) )
#define FE_WIDGET_REFRESH(widget, surface)							\
	( (FE_WIDGET_GET_TYPE(widget)->refresh)(FE_WIDGET(widget), FE_SURFACE(surface)) )
#define FE_WIDGET_CLEAR(widget, surface)							\
	( (FE_WIDGET_GET_TYPE(widget)->clear)(FE_WIDGET(widget), FE_SURFACE(surface)) )
#define FE_WIDGET_CONTROL(widget, cmd, va)							\
	( (FE_WIDGET_GET_TYPE(widget)->control)(FE_WIDGET(widget), cmd, va) )

typedef int (*fe_widget_write_t)(struct fe_widget *widget, const char *str, int len);
typedef int (*fe_widget_read_t)(struct fe_widget *widget, char *buffer, int max);
typedef void (*fe_widget_refresh_t)(struct fe_widget *widget, struct fe_surface *surface);
typedef void (*fe_widget_clear_t)(struct fe_widget *widget, struct fe_surface *surface);
typedef int (*fe_widget_control_t)(struct fe_widget *widget, int cmd, va_list va);

struct fe_layout;

struct fe_widget {
	struct object_s object;
	char *id;
	int bitflags;
	struct fe_widget *parent;
};

struct fe_widget_type {
	struct object_type_s object_type;
	fe_widget_write_t write;
	fe_widget_read_t read;
	fe_widget_refresh_t refresh;
	fe_widget_clear_t clear;
	fe_widget_control_t control;
};

extern struct fe_widget_type fe_widget_type;

int init_widget(void);
int release_widget(void);

int fe_widget_init(struct fe_widget *, const char *, va_list);
void fe_widget_release(struct fe_widget *);

struct fe_widget *fe_find_widget(const char *);
int fe_widget_control(struct fe_widget *, int, ...);
struct fe_widget *fe_widget_generate(struct fe_widget_type *, struct property_s *, struct fe_layout *);

#endif


