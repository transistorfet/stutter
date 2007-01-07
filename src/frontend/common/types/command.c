/*
 * Type Name:		command.c
 * Version:		0.1
 * Module Requirements:	type ; memory
 * Description:		Frontend Widget Command Type
 */

#include <string.h>
#include <stdarg.h>

#include <stutter/type.h>
#include <stutter/memory.h>
#include <stutter/frontend/widget.h>

typedef (*fe_common_command_func_t)(struct widget_s *, char *);

struct fe_common_command_s {
	fe_common_command_func_t func;
	struct widget_s *widget;
};

static void *fe_common_command_create(void *, char *, va_list);
static int fe_common_command_evaluate(void *, void *);
static void fe_common_command_destroy(void *);

struct type_s *fe_common_load_command(void)
{
	return(add_type(
		"command:fe",
		0,
		(type_create_t) fe_common_command_create,
		(type_destroy_t) fe_common_command_destroy,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(type_evaluate_t) fe_common_command_evaluate
	));
}

static void *fe_common_command_create(void *value, char *params, va_list va)
{
	struct fe_common_command_s *command;

	if (value)
		fe_common_command_destroy(value);

	if (strcmp(params, "callback,widget"))
		return(NULL);
	if (!(command = (struct fe_common_command_s *) memory_alloc(sizeof(struct fe_common_command_s))))
		return(NULL);
	command->func = va_arg(va, fe_common_command_func_t);
	command->widget = va_arg(va, struct widget_s *);
	va_end(va);
	return((void *) command);
}

static int fe_common_command_evaluate(void *data, void *ptr)
{
	struct fe_common_command_s *command;

	if ((command = data) && command->func)
		return(command->func(command->widget, ptr));
	return(-1);
}

static void fe_common_command_destroy(void *data)
{
	if (data)
		memory_free(data);
}

