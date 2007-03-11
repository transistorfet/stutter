/*
 * Type Name:		colour.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; string ; modbase
 * Description:		Colour Type
 */

#include <stdio.h>
#include <stdlib.h>

#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/string.h>
#include <stutter/frontend/surface.h>

static char *fe_common_colours[] = {
	"white",
	"black",
	"blue",
	"green",
	"red",
	"cyan",
	"magenta",
	"yellow",
	"bright yellow",
	"bright green",
	"bright cyan",
	"bright red",
	"bright blue",
	"bright magenta",
	"grey",
	"bright white"
};

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
	int i;
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
			value->enc = SC_ENC_MAPPING;
			if (!strcmp(str, "default"))
				value->colour = SC_MAP_DEFAULT_COLOUR;
			else if (!strcmp(str, "current"))
				value->colour = SC_MAP_CURRENT_COLOUR;
			else {
				for (i = 0;i < 16;i++) {
					if (!strcmp(str, fe_common_colours[i])) {
						value->colour = i;
						break;
					}
				}
			}
		}
	}
	return(value);
}

static int fe_common_colour_stringify(colour_t *value, char *buffer, int max)
{
	if (value->enc == SC_ENC_RGBA)
		return(snprintf(buffer, max, "#%06X", value->colour));
	else if (value->enc == SC_ENC_MAPPING)
		return(snprintf(buffer, max, "%d", value->colour));
	else
		return(0);
}


