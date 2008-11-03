/*
 * Header Name:		object.h
 * Description:		Base Object
 */

#ifndef _STUTTER_OBJECT_H
#define _STUTTER_OBJECT_H

#include <stdarg.h>

#define OBJECT_NOT_HANDLED		0
#define OBJECT_HANDLED			1
#define OBJECT_HANDLED_ALL		2

#define OBJECT_BF_RELEASING		0x1000

#define OBJECT_S(ptr)			( (struct object_s *) (ptr) )
#define OBJECT_TYPE_S(ptr)		( (struct object_type_s *) (ptr) )
#define OBJECT_GET_TYPE(ptr)		( OBJECT_TYPE_S(OBJECT_S(ptr)->type) )

struct object_s;
struct object_type_s;

/** Initialize the preallocated object using the given variable args.  The object will be zero'd before
    the initialization function is called.  If a negative number is returned, the object will destroyed
    and the release function for that object will be called. */
typedef int (*object_init_t)(struct object_s *, const char *, va_list);
/** Release the resources freed by the object */
typedef void (*object_release_t)(struct object_s *);

struct object_s {
	struct object_type_s *type;
	int bitflags;
};

struct object_type_s {
	struct object_type_s *parent;
	char *name;
	int size;
	void *ptr;
	object_init_t init;
	object_release_t release;
};

int init_object(void);
void release_object(void);

int object_register_type(struct object_type_s *);
int object_deregister_type(struct object_type_s *);
struct object_type_s *object_find_type(const char *, struct object_type_s *);

struct object_s *create_object(struct object_type_s *, const char *, ...);
struct object_s *create_object_real(struct object_type_s *, const char *, va_list);
void destroy_object(struct object_s *);

static inline int object_is_a(struct object_s *obj, struct object_type_s *type) {
	struct object_type_s *cur;

	for (cur = obj->type; cur; cur = cur->parent) {
		if (cur == type)
			return(1);
	}
	return(0);
}

#endif

