/*
 * Type Name:		status.c
 * Version:		0.1
 * Module Requirements:	type ; memory ; string ; modbase
 * Description:		Status (Stringify Callback) Type
 */

#include <stdlib.h>
#include <stdarg.h>

#include <type.h>
#include <lib/memory.h>
#include <lib/string.h>
#include <modules/base.h>

#define MAX_BUFFER		64

struct base_status_s {
	stringify_t func;
	void *ptr;
};

static void *base_status_create(char *, ...);
static string_t base_status_stringify(void *);
static void base_status_destroy(void *);

struct type_s *base_load_status(void)
{
	return(add_type("status", (create_t) base_status_create, (stringify_t) base_status_stringify, NULL, (destroy_t) base_status_destroy));
}

/*** Local Functions ***/

static void *base_status_create(char *str, ...)
{
	va_list va;
	struct base_status_s *status;

	if (strcmp(str, "%r%p"))
		return(NULL);

	va_start(va, str);
	if (!(status = (struct base_status_s *) memory_alloc(sizeof(struct base_status *))))
		return(NULL);
	status->func = va_arg(va, stringify_t);
	status->ptr = va_arg(va, void *);
	va_end(va);
	return(status);
}

static string_t base_status_stringify(void *ptr)
{
	if (!ptr)
		return(NULL);
	return(((struct base_status_s *) ptr)->func(((struct base_status_s *) ptr)->ptr));
}

static void base_status_destroy(void *ptr)
{
	if (ptr)
		memory_free(ptr);
}


