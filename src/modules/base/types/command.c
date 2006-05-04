/*
 * Type Name:		command.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; memory
 * Description:		Command Type
 */

#include <string.h>
#include <stdarg.h>

#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/lib/memory.h>

struct base_command_s {
	callback_t func;
	void *ptr;
};

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

static void *base_command_create(void *value, char *str, va_list va)
{
	void *ptr;
	callback_t func;
	struct base_command_s *command;

	if (value)
		base_command_destroy(value);

	if (strncmp(str, "%r%", 3) || ((str[3] != 'p') && (str[3] != 's') && (str[3] != 'd') && (str[3] != 'c')) || (str[4] != '\0'))
		return(NULL);
	if (!(command = (struct base_command_s *) memory_alloc(sizeof(struct base_command_s))))
		return(NULL);
	command->func = va_arg(va, callback_t);
	command->ptr = va_arg(va, void *);
	va_end(va);
	return((void *) command);
}

static int base_command_evaluate(void *data, void *ptr)
{
	struct base_command_s *command;

	if ((command = data) && command->func)
		return(command->func(command->ptr, ptr));
	return(-1);
}

static void base_command_destroy(void *data)
{
	memory_free(data);
}

