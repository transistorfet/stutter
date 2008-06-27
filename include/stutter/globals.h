/*
 * Header Name:		globals.h
 * Description:		Global Definitions
 */

#ifndef _STUTTER_GLOBALS_H
#define _STUTTER_GLOBALS_H

#include <string.h>

#define IO_COND_READ			0x0001
#define IO_COND_WRITE			0x0002
#define IO_COND_ERROR			0x0004

typedef void (*destroy_t)(void *);
typedef int (*compare_t)(void *, void *);
typedef int (*traverse_t)(void *, void *);
typedef int (*callback_t)(void *, void *);
typedef int (*stringify_t)(void *, char *, int);

struct callback_s {
	callback_t func;
	void *ptr;
};

#define EXECUTE_CALLBACK(callback, arg)	\
	if ((callback).func)			\
		((callback).func)((callback).ptr, arg)

struct property_s {
	char *name;
	char *value;
	struct property_s *next;
};

static inline char *get_property(struct property_s *prop, char *name)
{
	while (prop) {
		if (!strcmp(prop->name, name))
			return(prop->value);
		prop = prop->next;
	}
	return(NULL);
}

#define DECLARE_UNUSED(arg)	((void) arg)

#endif


