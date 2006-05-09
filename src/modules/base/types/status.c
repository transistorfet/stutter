/*
 * Type Name:		status.c
 * Version:		0.1
 * Module Requirements:	type ; memory ; string ; modbase
 * Description:		Status (Stringify Callback) Type
 */

#include <stdlib.h>
#include <stdarg.h>

#include <stutter/type.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/string.h>
#include <stutter/modules/base.h>

#define MAX_BUFFER		64

typedef char *(*status_t)(void *);

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

static void *base_status_create(void *value, char *str, va_list va)
{
	struct base_status_s *status;

	if (value)
		base_status_destroy(value);

	if (strcmp(str, "%r%p"))
		return(NULL);

	if (!(status = (struct base_status_s *) memory_alloc(sizeof(struct base_status *))))
		return(NULL);
	status->func = va_arg(va, status_t);
	status->ptr = va_arg(va, void *);
	va_end(va);
	return(status);
}

static int base_status_stringify(void *ptr, char *buffer, int max)
{
	string_t str;

	if (!ptr)
		return(-1);
	if (!(str = ((struct base_status_s *) ptr)->func(((struct base_status_s *) ptr)->ptr)))
		return(-1);
	strncpy(buffer, str, max - 1);
	buffer[max - 1] = '\0';
	return(strlen(buffer));	
}

static void base_status_destroy(void *ptr)
{
	if (ptr)
		memory_free(ptr);
}


