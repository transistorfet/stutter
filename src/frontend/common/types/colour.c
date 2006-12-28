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
#include <stutter/frontend/surface.h>

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
	colour_t *colour;

	if (!strcmp(params, "pointer"))
		return(va_arg(va, void *));
	else if (!value)
		return(NULL);
	else if (!strcmp(params, "string")) {
		str = va_arg(va, char *);
		colour = (colour_t *) value;
		if (str[0] == '#') {
			if  (strlen(str) == 7) {
				colour->enc = SC_ENC_RGBA;
				colour->colour = util_atoi(&str[1], 16);
			}
		}
		else {
			colour->enc = SC_ENC_MAPPING;
			colour->colour = util_atoi(str, 10);
		}
	}
	return(value);
}

static int fe_common_colour_stringify(void *value, char *buffer, int max)
{
	return(snprintf(buffer, max, "#%06X", *((int *) value)));
}


