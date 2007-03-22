/*
 * Type Name:		string-ref.c
 * Description:		String Reference Type
 */

#include <stdarg.h>
#include <string.h>

#include <stutter/type.h>
#include <stutter/string.h>
#include <stutter/modules/base.h>

static void *base_string_ref_create(char **, char *, va_list);
static int base_string_ref_stringify(char **, char *, int);

struct type_s *base_load_string_ref(void)
{
	return(add_type(
		"string-ref",
		0,
		(type_create_t) base_string_ref_create,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(type_stringify_t) base_string_ref_stringify,
		NULL
	));
}

static void *base_string_ref_create(char **value, char *params, va_list va)
{
	if (strcmp(params, "pointer"))
		return(NULL);
	value = va_arg(va, char **);
	return(value);
}

static int base_string_ref_stringify(char **value, char *buffer, int max)
{
	if (!value)
		return(-1);
	strncpy(buffer, *value, max - 1);
	buffer[max - 1] = '\0';
	return(strlen(buffer));
}


