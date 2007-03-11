/*
 * Header Name:		layout.h
 * Version:		0.1
 * Description:		Widget Layout Manager Interface
 */

#ifndef _STUTTER_FRONTEND_COMMON_LAYOUT_H
#define _STUTTER_FRONTEND_COMMON_LAYOUT_H

#include <stutter/frontend/widget.h>

#define LAYOUT_RT_WIDGET	0x0001
#define LYAOUT_RT_SURFACE	0x0002

struct layout_s;

typedef void *(*layout_create_t)(void *, struct widget_attrib_s *, struct layout_s *);

struct layout_type_s {
	char *name;
	int bitflags;
	layout_create_t func;
	void *ptr;
};

struct layout_s {
	struct layout_type_s *type;
	struct widget_attrib_s *attribs;
	struct layout_s *children;
	struct layout_s *next;
};

#define layout_call_create_m(type, attribs, children)	\
	type->func(type->ptr, attribs, children)

int init_layout(void);
int release_layout(void);

int layout_register_type(char *, int, layout_create_t, void *);
int layout_unregister_type(char *);
struct layout_type_s *layout_find_type(char *);

struct layout_s *make_layout(char *, struct widget_attrib_s *, struct layout_s *, struct layout_s *);
void destroy_layout(struct layout_s *);
struct widget_attrib_s *make_layout_attrib(char *, char *, struct widget_attrib_s *);
void destroy_layout_attrib(struct widget_attrib_s *);

int add_layout(char *, char *, struct layout_s *layout);
int remove_layout(char *, char *);
struct layout_s *find_layout(char *, char *);

void *layout_generate_object(char *, char *, int, char *);

#endif

