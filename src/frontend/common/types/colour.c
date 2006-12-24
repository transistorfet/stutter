/*
 * Type Name:		colour.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; string ; modbase
 * Description:		Colour Type
 */

#include <stdlib.h>

#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/lib/string.h>
#include <stutter/modules/base.h>

static void *fe_common_colour_create(void *, char *, va_list);
static int fe_common_colour_stringify(void *, char *, int);

struct type_s *fe_common_load_colour(void)
{
	return(add_type(
		"colour:fe",
		0,
		(type_create_t) fe_common_colour_create,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(type_stringify_t) fe_common_colour_stringify,
		NULL
	));
}

/*** Local Functions ***/

static void *fe_common_colour_create(void *value, char *params, va_list va)
{
	char *str;

	if (!strcmp(params, "pointer"))
		return(va_arg(va, void *));
	else if (!strcmp(params, "string")) {
		str = va_arg(va, char *);
		if ((str[0] == '#') && (strlen(str) == 7))
			*((int *) value) = util_atoi(&str[1], 16);
	}
	return(value);
}

static int fe_common_colour_stringify(void *value, char *buffer, int max)
{
	return(snprintf(buffer, max, "#%06X", *((int *) value)));
}


