/*
 * Header Name:		widget.h
 * Version:		0.1
 * Description:		Frontend Widget Header
 */

#ifndef _STUTTER_FRONTEND_WIDGET_H
#define _STUTTER_FRONTEND_WIDGET_H

#include <stdarg.h>
#include <string.h>

#include <stutter/globals.h>

#define WT_DRAWING		0x0004
#define WT_CONTAINER		0x1000
#define WT_FRAME		0x1001
#define WT_REGION		0x1002

#define WBF_NEEDS_REFRESH	0x0001
#define WBF_FORCE_REFRESH	0x0002

#define WCC_GET_SURFACE		0x0010
#define WCC_SET_SURFACE		0x0011
#define WCC_GET_WINDOW		0x0012
#define WCC_SET_WINDOW		0x0013
#define WCC_MODIFY_SIZE		0x0014
#define WCC_MODIFY_POSITION	0x0015
#define WCC_GET_MIN_MAX_SIZE	0x0016

#define WCC_ADD_WIDGET		0x0020
#define WCC_INSERT_WIDGET	0x0021
#define WCC_REMOVE_WIDGET	0x0022
#define WCC_CURRENT_WIDGET	0x0023
#define WCC_SELECT_WIDGET	0x0024
#define WCC_NEXT_WIDGET		0x0025
#define WCC_PREVIOUS_WIDGET	0x0026
#define WCC_FIRST_WIDGET	0x0027
#define WCC_LAST_WIDGET		0x0028

#define WCC_SCROLL		0x0030
#define WCC_INSERT_CHAR		0x0031
#define WCC_PROCESS_CHAR	0x0032

typedef struct widget_size_s {
	short width;
	short height;
} widget_size_t;

typedef struct widget_pos_s {
	short x;
	short y;
} widget_pos_t;

struct widget_s;

#define WIDGET_S(widget)	((struct widget_s *) (widget))

#define widget_init_m(widget, props)					\
	( (WIDGET_S(widget)->type->init)(WIDGET_S(widget), props) )
#define widget_release_m(widget)					\
	( (WIDGET_S(widget)->type->release)(WIDGET_S(widget)) )
#define widget_refresh_m(widget)					\
	( (WIDGET_S(widget)->type->refresh)(WIDGET_S(widget)) )
#define widget_print_m(widget, str, len)				\
	( (WIDGET_S(widget)->type->print)(WIDGET_S(widget), str, len) )
#define widget_clear_m(widget)						\
	( (WIDGET_S(widget)->type->clear)(WIDGET_S(widget)) )
#define widget_read_m(widget, buffer, max)				\
	( (WIDGET_S(widget)->type->read)(WIDGET_S(widget), buffer, max) )
#define widget_control_m(widget, cmd, va)				\
	( (WIDGET_S(widget)->type->control)(WIDGET_S(widget), cmd, va) )

typedef int (*widget_init_t)(struct widget_s *widget, struct property_s *props);
typedef int (*widget_release_t)(struct widget_s *widget);
typedef void (*widget_refresh_t)(struct widget_s *widget);
typedef int (*widget_print_t)(struct widget_s *widget, const char *str, int len);
typedef void (*widget_clear_t)(struct widget_s *widget);
typedef int (*widget_read_t)(struct widget_s *widget, char *buffer, int max);
typedef int (*widget_control_t)(struct widget_s *widget, int cmd, va_list va);

struct layout_s;

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
	int bitflags;
	struct widget_type_s *type;
	struct widget_s *parent;
};

int init_widget(void);
int release_widget(void);

int add_widget_type(struct widget_type_s *);
int remove_widget_type(char *);
struct widget_type_s *find_widget_type(char *);

struct widget_s *create_widget(struct widget_type_s *, char *, struct widget_s *, struct property_s *);
int destroy_widget(struct widget_s *);
struct widget_s *find_widget(char *);

int widget_control(struct widget_s *, int, ...);
struct widget_s *generate_widget(struct widget_type_s *, struct property_s *, struct layout_s *);

#endif


