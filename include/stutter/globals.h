/*
 * Header Name:		globals.h
 * Version:		0.1
 * Description:		Global Definitions
 */

#ifndef _STUTTER_GLOBALS_H
#define _STUTTER_GLOBALS_H

#include <string.h>

#define ERR_OUT_OF_MEMORY		-10
#define ERR_COMMAND_NOT_FOUND		-11

#define IO_COND_READ			0x01
#define IO_COND_WRITE			0x02
#define IO_COND_ERROR			0x04

typedef void (*destroy_t)(void *);
typedef int (*compare_t)(void *, void *);
typedef int (*traverse_t)(void *, void *);
typedef int (*callback_t)(void *, void *);

struct callback_s {
	callback_t func;
	void *ptr;
};

#define execute_callback_m(callback, arg)	\
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

#define declare_unused(arg)	((void) arg)

#endif


