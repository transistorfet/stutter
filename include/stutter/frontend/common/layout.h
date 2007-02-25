/*
 * Header Name:		layout.h
 * Version:		0.1
 * Description:		Widget Layout Manager Interface
 */

#ifndef _STUTTER_FRONTEND_COMMON_LAYOUT_H
#define _STUTTER_FRONTEND_COMMON_LAYOUT_H

#include <stutter/frontend/widget.h>

struct layout_s;

typedef struct widget_s *(*layout_create_t)(void *, struct widget_attrib_s *, struct layout_s *);

// TODO type doesn't seem like the right name here
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

// TODO this shouldn't take the layout
int add_layout(char *module, char *name, struct layout_s *layout);
int remove_layout(char *module, char *name);
struct layout_s *find_layout(char *module, char *name);

int layout_register_type(char *type, int, layout_create_t func, void *ptr);
int layout_unregister_type(char *type);
struct layout_type_s *layout_find_type(char *name);

struct widget_s *layout_generate_widget(char *module, char *name, char *id);
struct widget_s *layout_create_widget(struct widget_type_s *type, struct widget_attrib_s *attribs, struct layout_s *children);

#endif

