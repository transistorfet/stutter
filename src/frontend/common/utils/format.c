/*
 * Module Name:		format.c
 * Version:		0.1
 * Module Requirements:	memory ; surface
 * Description:		Format String Parser
 */

#include CONFIG_H
#include <stutter/lib/memory.h>
#include <stutter/lib/macros.h>
#include <stutter/frontend/surface.h>
#include "format.h"

static int format_colours[] = { 0x7F7F7F, 0x000000, 0x00007F, 0x007F00, 0x7F0000, 0x007F7F, 0x7F007F, 0x7F7F00,
				0xFFFF00, 0x00FF00, 0x00FFFF, 0xFF0000, 0x0000FF, 0xFF00FF, 0x7F7F7F, 0xFFFFFF };

static int format_string_parse(const char *, struct format_string_s *, int, int);

/**
 * Parse the given string into a string without special attribute characters
 * along with a list of the appropriate attributes and where they should
 * become active.  A pointer to the parsed string structure is returned or
 * NULL is returned on error.
 */
struct format_string_s *create_format_string(const char *str, int size)
{
	char buffer[STRING_SIZE];
	int i, num_styles, length;
	struct format_string_s tmp;
	struct format_string_s *format;
	struct format_style_s styles[FORMAT_MAX_STYLES];

	tmp.str = buffer;
	tmp.styles = styles;
	if ((num_styles = format_string_parse(str, &tmp, STRING_SIZE, FORMAT_MAX_STYLES)))
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

/*** Local Functions ***/

/**
 * Parse the given string for special style attribute characters.  The string
 * with the special characters removed is copied into the given buffer up to
 * the maximum number of characters.  Style attributes are stored in the given
 * array of styles up to the maximum.  The length of the buffer is stored in
 * the given int pointer, length, unless it is NULL.  The number of styles used
 * is returned or -1 is returned on error.
 */
static int format_string_parse(const char *str, struct format_string_s *format, int buffer_max, int styles_max)
{
	int i = 0, j = 0, k = 0;

	while (str[i] != '\0') {
		switch (str[i]) {
			case 0x02: {
				format->styles[k].index = j;
				format->styles[k].attrib.attrib = SA_TOGGLE | SA_BOLD;
				format->styles[k].attrib.fg = -1;
				format->styles[k].attrib.bg = -1;
				i++;
				if (++k >= styles_max)
					k--;
				break;
			}
			case 0x03: {
				int fg, bg = -1;

				if (++i && !is_number_char_m(str[i]))
					break;
				fg = str[i++] - 0x30;
				if (is_number_char_m(str[i]))
					fg = (fg * 10) + (str[i++] - 0x30);
				fg = format_colours[(fg >= 16) ? fg / 16 : fg];

				if ((str[i] == ',') && ++i && is_number_char_m(str[i])) {
					bg = str[i++] - 0x30;
					if (is_number_char_m(str[i]))
						bg = (bg * 10) + (str[i++] - 0x30);
					bg = format_colours[(bg >= 16) ? bg / 16 : bg];
				}
				format->styles[k].index = j;
				format->styles[k].attrib.attrib = SA_OR;
				format->styles[k].attrib.fg = fg;
				format->styles[k].attrib.bg = bg;
				if (++k >= styles_max)
					k--;
				break;
			}
			case 0x0F: {
				format->styles[k].index = j;
				format->styles[k].attrib.attrib = SA_SET | SA_NORMAL;
				// TODO is this supposed to reset the colour too?
				format->styles[k].attrib.fg = -1;
				format->styles[k].attrib.bg = -1;
				i++;
				if (++k >= styles_max)
					k--;
				break;
			}
			case 0x16: {
				format->styles[k].index = j;
				format->styles[k].attrib.attrib = SA_TOGGLE | SA_INVERSE;
				format->styles[k].attrib.fg = -1;
				format->styles[k].attrib.bg = -1;
				i++;
				if (++k >= styles_max)
					k--;
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


