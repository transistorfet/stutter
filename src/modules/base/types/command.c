/*
 * Type Name:		command.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; memory
 * Description:		Command Type
 */

#include <string.h>
#include <stdarg.h>

#include <stutter/type.h>
#include <stutter/memory.h>
#include <stutter/globals.h>
#include <stutter/variable.h>

static void *base_command_create(void *, char *, va_list);
static int base_command_evaluate(void *, void *);
static void base_command_destroy(void *);

struct type_s *base_load_command(void)
{
	return(add_type(
		"command",
		0,
		(type_create_t) base_command_create,
		(type_destroy_t) base_command_destroy,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(type_evaluate_t) base_command_evaluate
	));
}

static void *base_command_create(void *value, char *params, va_list va)
{
	void *ptr;
	callback_t func;
	struct callback_s *callback;

	if (value)
		base_command_destroy(value);

	if (strncmp(params, "callback,", 9) || strchr(&params[9], ','))
		return(NULL);
	if (!(callback = (struct callback_s *) memory_alloc(sizeof(struct callback_s))))
		return(NULL);
	callback->func = va_arg(va, callback_t);
	callback->ptr = va_arg(va, void *);
	va_end(va);
	return((void *) callback);
}

static int base_command_evaluate(void *value, void *ptr)
{
	struct callback_s *callback;

	if (callback = value)
		execute_callback_m(*callback, ptr);
	return(-1);
}

static void base_command_destroy(void *value)
{
	memory_free(value);
}

