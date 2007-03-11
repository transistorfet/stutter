/*
 * Type Name:		status.c
 * Version:		0.1
 * Module Requirements:	type ; memory ; string ; modbase
 * Description:		Status (Stringify Callback) Type
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <stutter/type.h>
#include <stutter/memory.h>
#include <stutter/string.h>
#include <stutter/modules/base.h>

#define MAX_BUFFER		64

typedef int (*status_t)(void *, char *, int);

struct base_status_s {
	status_t func;
	void *ptr;
};

static void *base_status_create(void *, char *, va_list);
static int base_status_stringify(void *, char *, int);
static void base_status_destroy(void *);

struct type_s *base_load_status(void)
{
	return(add_type(
		"status",
		0,
		(type_create_t) base_status_create,
		(type_destroy_t) base_status_destroy,
		NULL,
		NULL,
		NULL,
		NULL,
		(type_stringify_t) base_status_stringify,
		NULL
	));
}

/*** Local Functions ***/

static void *base_status_create(void *value, char *params, va_list va)
{
	struct base_status_s *status;

	if (value)
		base_status_destroy(value);

	if (strncmp(params, "stringify,", 10) || strchr(&params[10], ','))
		return(NULL);

	if (!(status = (struct base_status_s *) memory_alloc(sizeof(struct base_status *))))
		return(NULL);
	status->func = va_arg(va, status_t);
	status->ptr = va_arg(va, void *);
	va_end(va);
	return(status);
}

static int base_status_stringify(void *value, char *buffer, int max)
{
	if (!value)
		return(-1);
	return(((struct base_status_s *) value)->func(((struct base_status_s *) value)->ptr, buffer, max));
}

static void base_status_destroy(void *value)
{
	if (value)
		memory_free(value);
}


