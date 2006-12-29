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

static void *fe_common_colour_create(colour_t *, char *, va_list);
static int fe_common_colour_stringify(colour_t *, char *, int);

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

static void *fe_common_colour_create(colour_t *value, char *params, va_list va)
{
	char *str;

	if (!strcmp(params, "pointer"))
		return(va_arg(va, void *));
	else if (!value)
		return(NULL);
	else if (!strcmp(params, "string")) {
		str = va_arg(va, char *);
		if (str[0] == '\0') {
			value->enc = SC_ENC_MAPPING;
			value->colour = SC_MAP_CURRENT_COLOUR;
		}
		else if (str[0] == '#') {
			if  (strlen(str) == 7) {
				value->enc = SC_ENC_RGBA;
				value->colour = util_atoi(&str[1], 16);
			}
		}
		else if ((str[0] >= 0x30) && (str[0] <= 0x39)) {
			value->enc = SC_ENC_MAPPING;
			value->colour = util_atoi(str, 10);
		}
		else {
			// TODO check for colour names here
			value->enc = SC_ENC_MAPPING;
			value->colour = SC_MAP_DEFAULT_COLOUR;
		}
	}
	return(value);
}

static int fe_common_colour_stringify(colour_t *value, char *buffer, int max)
{
	return(snprintf(buffer, max, "#%06X", value->colour));
}


