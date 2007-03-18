/*
 * Header Name:		layout.h
 * Version:		0.1
 * Description:		Widget Layout Manager Interface
 */

#ifndef _STUTTER_FRONTEND_COMMON_LAYOUT_H
#define _STUTTER_FRONTEND_COMMON_LAYOUT_H

#include <stutter/globals.h>

#define LAYOUT_RT_WIDGET	0x0001
#define LAYOUT_RT_SURFACE	0x0002
#define LAYOUT_RT_MENU		0x0003

#define LAYOUT_RETURN_TYPE(type)	( (type)->bitflags )

struct layout_s;

typedef void *(*layout_create_t)(void *, struct property_s *, struct layout_s *);

struct layout_type_s {
	char *name;
	int bitflags;
	layout_create_t func;
	void *ptr;
};

struct layout_s {
	struct layout_type_s *type;
	struct property_s *props;
	struct layout_s *children;
	struct layout_s *next;
};

#define layout_call_create_m(type, props, children)	\
	type->func(type->ptr, props, children)

int init_layout(void);
int release_layout(void);

int layout_register_type(char *, int, layout_create_t, void *);
int layout_unregister_type(char *);
struct layout_type_s *layout_find_type(char *);

struct layout_s *make_layout(char *, struct property_s *, struct layout_s *, struct layout_s *);
void destroy_layout(struct layout_s *);
struct property_s *make_layout_property(char *, char *, struct property_s *);
void destroy_layout_property(struct property_s *);

int add_layout(char *, char *, struct layout_s *layout);
int remove_layout(char *, char *);
struct layout_s *find_layout(char *, char *);

void *layout_generate_object(char *, char *, int, char *);

#endif

