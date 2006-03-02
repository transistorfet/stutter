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

static void *base_command_create(char *, ...);
static int base_command_evaluate(void *, void *);
static void base_command_destroy(void *);

struct type_s *base_load_command(void)
{
	return(add_type("command", (create_t) base_command_create, NULL, (evaluate_t) base_command_evaluate, (destroy_t) base_command_destroy));
}

static void *base_command_create(char *str, ...)
{
	void *ptr;
	va_list va;
	callback_t func;
	struct base_command_s *command;

	if (strncmp(str, "%r%", 3) || ((str[3] != 'p') && (str[3] != 's') && (str[3] != 'd') && (str[3] != 'c')) || (str[4] != '\0'))
		return(NULL);
	if (!(command = (struct base_command_s *) memory_alloc(sizeof(struct base_command_s))))
		return(NULL);
	va_start(va, str);
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

