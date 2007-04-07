/*
 * Widget Name:		text.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; surface ; window
 * Description:		Text Window Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/memory.h>
#include <stutter/macros.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "text.h"
#include "../utils/format.h"

#define TEXT_MAX_WRAP		20

struct widget_type_s text_type = {
	"text",
	0,
	sizeof(struct text_s),
	(widget_init_t) text_init,
	(widget_release_t) text_release,
	(widget_refresh_t) text_refresh,
	(widget_print_t) text_print,
	(widget_clear_t) text_clear,
	(widget_read_t) text_read,
	(widget_control_t) text_control
};

extern struct variable_table_s *fe_theme;

static int text_wrap_string(char *, int);

int text_init(struct text_s *text, struct property_s *props)
{
	text->cur_line = 0;
	text->max_lines = FE_TEXT_LOG_SIZE;
	queue_init_v(text->log, 0);
	return(0);
}

int text_release(struct text_s *text)
{
	struct text_entry_s *cur, *tmp;

	queue_foreach_safe_v(text->log, log, cur, tmp) {
		memory_free(cur);
	}
	return(0);
 }

int text_refresh(struct text_s *text)
{
	int line = 0;
	attrib_t attrib;
	widget_pos_t pos;
	widget_size_t size;
	int i, j, k, max, limit;
	char buffer[LARGE_STRING_SIZE];
	struct surface_s *surface;
	struct text_entry_s *cur;
	int indices[TEXT_MAX_WRAP];
	struct format_string_s format;
	struct format_style_s styles[FE_FORMAT_MAX_STYLES];

	WIDGET_S(text)->bitflags &= ~WBF_NEEDS_REFRESH;
	widget_control(WIDGET_S(text)->parent, WCC_GET_SURFACE, &surface);
	widget_control(WIDGET_S(text)->parent, WCC_GET_WINDOW, &pos, &size);

	line = size.height;
	surface_clear_m(surface, pos.x, pos.y, size.width, size.height);
	surface_move_m(surface, pos.x, pos.y + line);

	if (!(cur = queue_first_v(text->log)))
		return(0);
	for (j = 0;j < text->cur_line;j++) {
		if (!(cur = queue_next_v(log, cur)))
			return(0);
	}

	while (line >= 0) {
		limit = size.width;
		format.str = buffer;
		format.styles = styles;
		if (parse_format_string(fe_theme, cur->str, &format, LARGE_STRING_SIZE, FE_FORMAT_MAX_STYLES))
			continue;
		for (i = j = 0;j < TEXT_MAX_WRAP;j++) {
			indices[j] = text_wrap_string(&format.str[i], limit);
			if (indices[j] == -1)
				break;
			if (!j)
				limit = size.width - strlen(FE_TEXT_WRAP_STRING);
			i += indices[j];
		}
		max = (j == TEXT_MAX_WRAP) ? j : j + 1;

		line -= max;
		surface_control_m(surface, SCC_GET_ATTRIB, &attrib);
		for (i = j = 0;(j < max) && (line + j < 0);j++)
			i += indices[j];
		for (k = 0;(k < format.num_styles) && (format.styles[k].index < i);k++)
			surface_control_m(surface, SCC_SET_ATTRIB, &format.styles[k].attrib);
		for (;j < max;j++) {
			surface_move_m(surface, pos.x, pos.y + line + j);
			if (j)
				surface_print_m(surface, FE_TEXT_WRAP_STRING, -1);
			if (indices[j] == -1)
				limit = format.length;
			else
				limit = indices[j] + i;
			for (;(k < format.num_styles) && (format.styles[k].index < limit);k++) {
				surface_print_m(surface, &format.str[i], format.styles[k].index - i);
				surface_control_m(surface, SCC_SET_ATTRIB, &format.styles[k].attrib);
				i = format.styles[k].index;
			}
			surface_print_m(surface, &format.str[i], limit - i);
			i = limit;
		}
		surface_control_m(surface, SCC_SET_ATTRIB, &attrib);
		if (!(cur = queue_next_v(log, cur)))
			break;
	}
	return(0);
}

int text_print(struct text_s *text, const char *str, int len)
{
	struct text_entry_s *node;

	if (!text || !str)
		return(-1);
	if (len == -1)
		len = strlen(str);
	/** If the scroll is not at the bottom then make sure the screen doesn't move */
	if (text->cur_line && (text->cur_line < text->max_lines))
		text->cur_line++;

	if (!(node = (struct text_entry_s *) memory_alloc(sizeof(struct text_entry_s) + len + 1)))
		return(-1);
	node->str = (char *) offset_after_struct_m(node, 0);
	strncpy(node->str, str, len);
	node->str[len] = '\0';

	queue_prepend_node_v(text->log, log, node);
	if (queue_size_v(text->log) > text->max_lines) {
		queue_pop_node_v(text->log, log, node);
		memory_free(node);
	}
	WIDGET_S(text)->bitflags |= WBF_NEEDS_REFRESH;

	return(len);
}

void text_clear(struct text_s *text)
{
	struct text_entry_s *cur, *tmp;

	if (!text)
		return;
	queue_foreach_safe_v(text->log, log, cur, tmp) {
		memory_free(cur);
	}
	queue_release_v(text->log);
	WIDGET_S(text)->bitflags |= WBF_NEEDS_REFRESH;
}

int text_read(struct text_s *text, char *buffer, int max)
{
	return(-1);
}

int text_control(struct text_s *text, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SET_PARENT: {
			struct widget_s *parent;

			parent = va_arg(va, struct widget_s *);
			WIDGET_S(text)->parent = parent;
			return(0);
		}
		case WCC_SET_WINDOW: {
			WIDGET_S(text)->bitflags |= WBF_NEEDS_REFRESH;
			return(0);
		}
		case WCC_SCROLL: {
			int amount;

			amount = va_arg(va, int);
			text->cur_line += amount;
			if (text->cur_line < 0)
				text->cur_line = 0;
			else if (text->cur_line > queue_size_v(text->log))
				text->cur_line = queue_size_v(text->log);
			WIDGET_S(text)->bitflags |= WBF_NEEDS_REFRESH;
			return(0);
		}
		default:
			break;
	}
	return(-1);
}

/*** Local Functions ***/

/**
 * Determine the index into the given string at which to break the string
 * in two given the maximum length of a each line.  If the line does not
 * need to be broken up, 0 is returned.
 */
static int text_wrap_string(char *str, int limit)
{
	int i;

	if (strlen(str) <= limit)
		return(-1);
	i = limit;
	while ((i > 0) && (str[i] != 0x20))
		i--;
	if (i == 0)
		return(limit);
	return(i);
}



