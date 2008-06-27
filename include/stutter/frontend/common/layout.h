/*
 * Header Name:		layout.h
 * Description:		Widget Layout Manager Interface
 */

#ifndef _STUTTER_FRONTEND_COMMON_LAYOUT_H
#define _STUTTER_FRONTEND_COMMON_LAYOUT_H

#include <stutter/globals.h>

#define LAYOUT_RT_WIDGET	0x0001
#define LAYOUT_RT_SURFACE	0x0002
#define LAYOUT_RT_MENU		0x0003

#define LAYOUT_RETURN_TYPE(type)	( (type)->bitflags )

struct fe_layout;

typedef void *(*fe_layout_create_t)(void *, struct property_s *, struct fe_layout *);

struct fe_layout_type {
	int bitflags;
	fe_layout_create_t func;
	void *ptr;
};

struct fe_layout {
	struct fe_layout_type *type;
	struct property_s *props;
	struct fe_layout *children;
	struct fe_layout *next;
};

#define FE_LAYOUT_CALL_CREATE(type, props, children)	\
	type->func(type->ptr, props, children)

int init_layout(void);
int release_layout(void);

int layout_register_type(char *, int, fe_layout_create_t, void *);
int layout_unregister_type(char *);
struct fe_layout_type *layout_find_type(char *);

struct fe_layout *make_layout(char *, struct property_s *, struct fe_layout *, struct fe_layout *);
void destroy_layout(struct fe_layout *);
struct property_s *make_layout_property(char *, char *, struct property_s *);
void destroy_layout_property(struct property_s *);

int add_layout(char *, char *, struct fe_layout *layout);
int remove_layout(char *, char *);
struct fe_layout *find_layout(char *, char *);

void *layout_generate_object(char *, char *, int, char *);

#endif

