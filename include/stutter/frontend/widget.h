/*
 * Header Name:		widget.h
 * Version:		0.1
 * Description:		Frontend Widget Header
 */

#ifndef _STUTTER_FRONTEND_WIDGET_H
#define _STUTTER_FRONTEND_WIDGET_H

#include <stdarg.h>
#include <stutter/frontend/surface.h>

#define WT_DRAWING		0x20

#define WCC_GET_CHAR		0x02

#define WCC_GET_SURFACE		0x10
#define WCC_SET_SURFACE		0x11
#define WCC_GET_SIZE		0x12
#define WCC_MODIFY_SIZE		0x13
#define WCC_GET_POSITION	0x14
#define WCC_MODIFY_POSITION	0x15

#define WCC_ADD_WIDGET		0x20
#define WCC_REMOVE_WIDGET	0x21
#define WCC_CURRENT_WIDGET	0x22
#define WCC_SELECT_WIDGET	0x23
#define WCC_NEXT_WIDGET		0x24
#define WCC_PREVIOUS_WIDGET	0x25
#define WCC_FIRST_WIDGET	0x26
#define WCC_LAST_WIDGET		0x27

#define WCC_SCROLL		0x30
#define WCC_INSERT_CHAR		0x31
#define WCC_PROCESS_CHAR	0x32

struct widget_s;

#define widget_init_m(widget, bitflags)					\
	((struct widget_s *) (widget))->type->init(((struct widget_s *) (widget)), bitflags)
#define widget_release_m(widget)					\
	( ((struct widget_s *) (widget))->type->release(((struct widget_s *) (widget))) )
#define widget_refresh_m(widget)					\
	( ((struct widget_s *) (widget))->type->refresh(((struct widget_s *) (widget))) )
#define widget_print_m(widget, str, len)				\
	( ((struct widget_s *) (widget))->type->print(((struct widget_s *) (widget)), str, len) )
#define widget_clear_m(widget)						\
	( ((struct widget_s *) (widget))->type->clear(((struct widget_s *) (widget))) )
#define widget_read_m(widget, buffer, max)				\
	( ((struct widget_s *) (widget))->type->read(((struct widget_s *) widget), buffer, max) )
#define widget_control_m(widget, cmd, va)				\
	( ((struct widget_s *) (widget))->type->control(((struct widget_s *) (widget)), cmd, va) )

// TODO how do we pass in special parameters?  have an array of strings?
typedef int (*widget_init_t)(struct widget_s *widget);
typedef int (*widget_release_t)(struct widget_s *widget);
typedef void (*widget_refresh_t)(struct widget_s *widget);
typedef int (*widget_print_t)(struct widget_s *widget, const char *str, int len);
typedef void (*widget_clear_t)(struct widget_s *widget);
typedef int (*widget_read_t)(struct widget_s *widget, char *buffer, int max);
typedef int (*widget_control_t)(struct widget_s *widget, int cmd, va_list va);

struct widget_type_s {
	char *name;
	short type;
	short size;
	widget_init_t init;
	widget_release_t release;
	widget_refresh_t refresh;
	widget_print_t print;
	widget_clear_t clear;
	widget_read_t read;
	widget_control_t control;
};

struct widget_s {
	char *id;
	struct widget_type_s *type;
	struct widget_s *parent;
};

struct widget_s *create_widget(struct widget_type_s *, char *, struct widget_s *, ...);
int destroy_widget(struct widget_s *);
int widget_control(struct widget_s *, int, ...);

#endif


