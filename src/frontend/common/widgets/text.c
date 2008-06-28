/*
 * Widget Name:		text.c
 * Description:		Text Window Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/memory.h>
#include <stutter/string.h>
#include <stutter/macros.h>
#include <stutter/globals.h>
#include <stutter/frontend/common/format.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/text.h>

#define TEXT_MAX_WRAP		20

struct fe_widget_type fe_text_type = { {
	OBJECT_TYPE_S(&fe_widget_type),
	"fe_text",
	sizeof(struct fe_text),
	NULL,
	(object_init_t) fe_text_init,
	(object_release_t) fe_text_release },
	(fe_widget_write_t) fe_text_write,
	(fe_widget_read_t) fe_text_read,
	(fe_widget_refresh_t) fe_text_refresh,
	(fe_widget_clear_t) fe_text_clear,
	(fe_widget_control_t) fe_text_control
};

extern struct variable_table_s *fe_theme;

static int fe_text_wrap_string(char *, int);

int fe_text_init(struct fe_text *text, const char *params, va_list va)
{
	if (fe_widget_init(FE_WIDGET(text), params, va))
		return(-1);
	text->cur_line = 0;
	text->max_lines = FE_TEXT_LOG_SIZE;
	if (!(text->log = create_queue(0, (destroy_t) destroy_string)))
		return(-1);
	return(0);
}

void fe_text_release(struct fe_text *text)
{
	if (text->log)
		destroy_queue(text->log);
}

int fe_text_write(struct fe_text *text, const char *str, int len)
{
	string_t string;

	if (!text || !str)
		return(-1);
	/** If the scroll is not at the bottom then make sure the screen doesn't move */
	if (text->cur_line && (text->cur_line < text->max_lines))
		text->cur_line++;

	if (!(string = create_string("%s", str)))
		return(-1);
	queue_prepend(text->log, string);
	if (QUEUE_SIZE(text->log) > text->max_lines) {
		if ((string = queue_pop(text->log)))
			destroy_string(string);
	}
	FE_WIDGET(text)->bitflags |= WBF_NEEDS_REFRESH;
	return(len);
}

int fe_text_read(struct fe_text *text, char *buffer, int max)
{
	return(-1);
}

int fe_text_refresh(struct fe_text *text, struct fe_surface *surface)
{
	int line = 0;
	string_t str;
	attrib_t attrib;
	widget_pos_t pos;
	widget_size_t size;
	int i, j, k, max, limit;
	char buffer[LARGE_STRING_SIZE];
	int indices[TEXT_MAX_WRAP];
	struct format_string_s format;
	struct format_style_s styles[FE_FORMAT_MAX_STYLES];

	FE_WIDGET(text)->bitflags &= ~WBF_NEEDS_REFRESH;
	fe_widget_control(FE_WIDGET(text)->parent, WCC_GET_WINDOW, &pos, &size);

	line = size.height;
	FE_SURFACE_CLEAR(surface, pos.x, pos.y, size.width, size.height);
	FE_SURFACE_MOVE(surface, pos.x, pos.y + line);

	if (!(str = (string_t) QUEUE_FIRST(text->log)))
		return(0);
	for (j = 0; j < text->cur_line; j++) {
		if (!(str = (string_t) QUEUE_NEXT(text->log)))
			return(0);
	}

	while (line >= 0) {
		limit = size.width;
		format.str = buffer;
		format.styles = styles;
		if (parse_format_string(fe_theme, str, &format, LARGE_STRING_SIZE, FE_FORMAT_MAX_STYLES))
			continue;
		for (i = j = 0; j < TEXT_MAX_WRAP; j++) {
			indices[j] = fe_text_wrap_string(&format.str[i], limit);
			if (indices[j] == -1)
				break;
			if (!j)
				limit = size.width - strlen(FE_TEXT_WRAP_STRING);
			i += indices[j];
		}
		max = (j == TEXT_MAX_WRAP) ? j : j + 1;

		line -= max;
		FE_SURFACE_CONTROL(surface, SCC_GET_ATTRIB, &attrib);
		for (i = j = 0; (j < max) && (line + j < 0); j++)
			i += indices[j];
		for (k = 0; (k < format.num_styles) && (format.styles[k].index < i); k++)
			FE_SURFACE_CONTROL(surface, SCC_SET_ATTRIB, &format.styles[k].attrib);
		for (; j < max; j++) {
			FE_SURFACE_MOVE(surface, pos.x, pos.y + line + j);
			if (j)
				FE_SURFACE_PRINT(surface, FE_TEXT_WRAP_STRING, -1);
			if (indices[j] == -1)
				limit = format.length;
			else
				limit = indices[j] + i;
			for (; (k < format.num_styles) && (format.styles[k].index < limit); k++) {
				FE_SURFACE_PRINT(surface, &format.str[i], format.styles[k].index - i);
				FE_SURFACE_CONTROL(surface, SCC_SET_ATTRIB, &format.styles[k].attrib);
				i = format.styles[k].index;
			}
			FE_SURFACE_PRINT(surface, &format.str[i], limit - i);
			i = limit;
		}
		FE_SURFACE_CONTROL(surface, SCC_SET_ATTRIB, &attrib);
		if (!(str = (string_t) QUEUE_NEXT(text->log)))
			break;
	}
	return(0);
}

void fe_text_clear(struct fe_text *text, struct fe_surface *surface)
{
	queue_clear(text->log);
	FE_WIDGET(text)->bitflags |= WBF_NEEDS_REFRESH;
}

int fe_text_control(struct fe_text *text, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SET_PARENT: {
			struct fe_widget *parent;

			parent = va_arg(va, struct fe_widget *);
			FE_WIDGET(text)->parent = parent;
			return(0);
		}
		case WCC_SET_WINDOW: {
			FE_WIDGET(text)->bitflags |= WBF_NEEDS_REFRESH;
			return(0);
		}
		case WCC_SCROLL: {
			int amount;

			amount = va_arg(va, int);
			text->cur_line += amount;
			if (text->cur_line < 0)
				text->cur_line = 0;
			else if (text->cur_line > QUEUE_SIZE(text->log))
				text->cur_line = QUEUE_SIZE(text->log);
			FE_WIDGET(text)->bitflags |= WBF_NEEDS_REFRESH;
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
static int fe_text_wrap_string(char *str, int limit)
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



