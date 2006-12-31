/*
 * Module Name:		format.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; memory ; surface
 * Description:		Format String Parser
 */

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/macros.h>
#include <stutter/frontend/surface.h>
#include "format.h"

/**
 * Parse the given string into a string without special attribute characters
 * along with a list of the appropriate attributes and where they should
 * become active.  A pointer to the parsed string structure is returned or
 * NULL is returned on error.
 */
struct format_string_s *create_format_string(struct variable_table_s *theme, const char *str, int size)
{
	char buffer[LARGE_STRING_SIZE];
	struct format_string_s tmp;
	struct format_string_s *format;
	struct format_style_s styles[FE_FORMAT_MAX_STYLES];

	tmp.str = buffer;
	tmp.styles = styles;
	if (parse_format_string(theme, str, &tmp, LARGE_STRING_SIZE, FE_FORMAT_MAX_STYLES))
		return(NULL);
	if (!size)
		size = sizeof(struct format_string_s);
	if (!(format = (struct format_string_s *) memory_alloc(size + (tmp.length + 1) + (sizeof(struct format_style_s) * tmp.num_styles))))
		return(NULL);
	format->str = (char *) (((size_t) format) + size);
	format->styles = (struct format_style_s *) (((size_t) format) + size + tmp.length + 1);
	copy_format_string(&tmp, format, tmp.length + 1, tmp.num_styles);
	return(format);
}

int destroy_format_string(struct format_string_s *format)
{
	memory_free(format);
	return(0);
}

int copy_format_string(struct format_string_s *src, struct format_string_s *dest, int buffer_max, int styles_max)
{
	int i;

	dest->length = src->length;
	strncpy(dest->str, src->str, buffer_max - 1);
	dest->str[buffer_max - 1] = '\0';
	dest->num_styles = src->num_styles;
	for (i = 0;(i < styles_max) && (i < src->num_styles);i++)
		dest->styles[i] = src->styles[i];
	return(0);
}

/**
 * Parse the given string for special style attribute characters.  The string
 * with the special characters removed is copied into the given buffer up to
 * the maximum number of characters.  Style attributes are stored in the given
 * array of styles up to the maximum.  The length of the buffer is stored in
 * the given int pointer, length, unless it is NULL.  The number of styles used
 * is returned or -1 is returned on error.
 */
int parse_format_string(struct variable_table_s *theme, const char *str, struct format_string_s *format, int buffer_max, int styles_max)
{
	int i = 0, j = 0, k = 0;

	while (str[i] != '\0') {
		switch (str[i]) {
			case 0x01: {
				i++;
				format->styles[k].index = j;
				i += util_decode_bytes(&str[i], (unsigned char *) &format->styles[k].attrib, sizeof(attrib_t));
				if (++k >= styles_max)
					k--;
				break;
			}
			case 0x12: {
				int ni;
				void *value;
				struct type_s *type;
				char name[NAME_STRING_SIZE];

				i++;
				for (ni = 0;(ni < NAME_STRING_SIZE) && (str[i] != 0x12);i++, ni++)
					name[ni] = str[i];
				name[ni] = '\0';
				i++;
				if ((value = find_variable(theme, name, &type)) && type && !strcmp(type->name, "attrib:fe")) {
					format->styles[k].attrib = *((attrib_t *) value);
					format->styles[k].index = j;
					if (++k >= styles_max)
						k--;
				}
				break;
			}
			default: {
				format->str[j++] = str[i++];
				if (j >= buffer_max)
					return(-1);
				break;
			}
		}
	}
	format->str[j] = '\0';
	format->length = j;
	format->num_styles = k;
	return(0);
}



