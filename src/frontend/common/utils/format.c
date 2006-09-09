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

// TODO how do you make the proper colours??
static int format_colours[] = { SA_WHITE, SA_BLACK, SA_BLUE, SA_GREEN, SA_RED, SA_CYAN, SA_MAGENTA, SA_YELLOW,
				SA_YELLOW, SA_GREEN, SA_CYAN, SA_CYAN, SA_BLUE, SA_MAGENTA, SA_WHITE, SA_WHITE };

static int format_string_parse(char *, char *, int, struct format_style_s *, int, int *);

/**
 * Parse the given string into a string without special attribute characters
 * along with a list of the appropriate attributes and where they should
 * become active.  A pointer to the parsed string structure is returned or
 * NULL is returned on error.
 */
struct format_string_s *create_format_string(char *str)
{
	char buffer[STRING_SIZE];
	int i, num_styles, length;
	struct format_string_s *format;
	struct format_style_s styles[FORMAT_MAX_STYLES];

	if ((num_styles = format_string_parse(str, buffer, STRING_SIZE, styles, FORMAT_MAX_STYLES, &length)) == -1)
		return(NULL);
	if (!(format = (struct format_string_s *) memory_alloc(sizeof(struct format_string_s) + (length + 1) + (sizeof(struct format_style_s) * num_styles))))
		return(NULL);
	format->length = length;
	format->str = (char *) (((size_t) format) + sizeof(struct format_string_s));
	strcpy(format->str, buffer);
	format->num_styles = num_styles;
	format->styles = (struct format_style_s *) (((size_t) format) + sizeof(struct format_string_s) + length + 1);
	for (i = 0;i < num_styles;i++)
		format->styles[i] = styles[i];
	return(format);
}

int destroy_format_string(struct format_string_s *format)
{
	memory_free(format);
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
static int format_string_parse(char *str, char *buffer, int buffer_max, struct format_style_s *styles, int styles_max, int *length)
{
	int i = 0, j = 0, k = 0;

	while (str[i] != '\0') {
		switch (str[i]) {
			case 0x02: {
				styles[k].index = j;
				styles[k].attrib.attrib = SA_TOGGLE | SA_BOLD;
				styles[k].attrib.fg = -1;
				styles[k].attrib.bg = -1;
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
				styles[k].index = j;
				styles[k].attrib.attrib = SA_OR;
				styles[k].attrib.fg = fg;
				styles[k].attrib.bg = bg;
				if (++k >= styles_max)
					k--;
				break;
			}
			case 0x16: {
				styles[k].index = j;
				styles[k].attrib.attrib = SA_TOGGLE | SA_INVERSE;
				styles[k].attrib.fg = -1;
				styles[k].attrib.bg = -1;
				i++;
				if (++k >= styles_max)
					k--;
				break;
			}
			default: {
				buffer[j++] = str[i++];
				if (j >= buffer_max)
					return(-1);
				break;
			}
		}
	}
	buffer[j] = '\0';
	if (length)
		*length = j;
	return(k);

/*
	int j;
	int i = 0, k = 0;

	// TODO what the fsck do you do about multiple attribs (bold & inverse at the same time for example?)
	format[0].attrib.fg = -1;
	format[0].attrib.bg  = -1;
	format[0].attrib.attrib = 0;
	while (1) {
		format[k].str = &str[i];
		for (j = i;(str[j] != '\0') && format_is_not_char_m(str[j]);j++) ;
		format[k].len = j - i;
		i = j;
		if (format[k].len) {
			if (++k >= FORMAT_MAX_STRINGS)
				break;
			format[k].attrib = format[k - 1].attrib;
		}
		if (str[i] == '\0')
			break;
		switch (str[i++]) {
			case 0x02: {
				if (format[k].attrib.attrib & SA_BOLD)
					format[k].attrib.attrib &= ~SA_BOLD;
				else
					format[k].attrib.attrib |= SA_BOLD;
				break;
			}
			case 0x03: {
				// TODO colour
				break;
			}
			case 0x16: {
				if (format[k].attrib.attrib & SA_INVERSE)
					format[k].attrib.attrib &= ~SA_INVERSE;
				else
					format[k].attrib.attrib |= SA_INVERSE;
				break;
			}
			default:
				break;
		}
	}
	return(k);
*/
}


