/*
 * Type Name:		command.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; callback
 * Description:		Command Type
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <type.h>
#include <variable.h>
#include <nit/callback.h>

static void *base_command_create(char *, ...);

int base_load_command(void)
{
	if (!(add_type("command", (create_t) base_command_create, (evaluate_t) execute_callback, (destroy_t) destroy_callback)))
		return(-1);
	return(0);
}

static void *base_command_create(char *str, ...)
{
	void *ptr;
	va_list va;
	callback_t func;

	if (!strncmp(str, "%r%", 3) && ((str[3] == 'p') || (str[3] == 's') || (str[3] == 'd') || (str[3] == 'c')) && (str[4] == '\0')) {
		va_start(va, str);
		func = va_arg(va, callback_t);
		ptr = va_arg(va, void *);
		return((void *) create_callback(0, 0, NULL, func, ptr));
	}
	else
		return(NULL);
}

