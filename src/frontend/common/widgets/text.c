/*
 * Widget Name:		scroll.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; surface ; window
 * Description:		Scrolling Window Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/lib/queue.h>
#include <stutter/lib/memory.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "scroll.h"
#include "../utils/format.h"

#define WINDOW_MAX_WRAP		20

#define WINDOW_MAX_LINES	100

struct widget_type_s scroll_type = {
	"scrolling-window",
	0,
	sizeof(struct scroll_s),
	(widget_init_t) scroll_init,
	(widget_release_t) scroll_release,
	(widget_refresh_t) scroll_refresh,
	(widget_print_t) scroll_print,
	(widget_clear_t) scroll_clear,
	(widget_read_t) scroll_read,
	(widget_control_t) scroll_control
};

static int scroll_wrap_string(char *, int);

int scroll_init(struct scroll_s *scroll)
{
	if (!scroll->widget.parent)
		return(-1);
	scroll->cur_line = 0;
	scroll->max_lines = WINDOW_MAX_LINES;
	queue_init_v(scroll->log);
	return(0);
}

int scroll_release(struct scroll_s *scroll)
{
	queue_destroy_v(scroll->log, log,
		memory_free(cur);
	);
	return(0);
 }

int scroll_refresh(struct scroll_s *scroll)
{
	int line = 0;
	attrib_t attrib;
	int x, y, width, height;
	int i, j, k, max, limit;
	struct surface_s *surface;
	struct scroll_entry_s *cur;
	int indices[WINDOW_MAX_WRAP];

	widget_control(scroll->widget.parent, WCC_GET_SURFACE, &surface);
	widget_control(scroll->widget.parent, WCC_GET_SIZE, &width, &height);
	widget_control(scroll->widget.parent, WCC_GET_POSITION, &x, &y);

	line = height;
	surface_clear_m(surface, x, y, width, height);
	surface_move_m(surface, x, y + line);

	if (!(cur = queue_first_v(scroll->log)))
		return(0);
	for (j = 0;j < scroll->cur_line;j++) {
		if (!(cur = queue_next_v(log, cur)))
			return(0);
	}

	while (line >= 0) {
		limit = width;
		for (i = j = 0;j < WINDOW_MAX_WRAP;j++) {
			indices[j] = scroll_wrap_string(&cur->format->str[i], limit);
			if (indices[j] == -1)
				break;
			if (!j)
				limit = width - strlen(FE_WINDOW_WRAP_STRING);
			i += indices[j];
		}
		max = (j == WINDOW_MAX_WRAP) ? j : j + 1;

		line -= max;
		surface_control_m(surface, SCC_GET_ATTRIB, &attrib);
		for (i = j = 0;(j < max) && (line + j < 0);j++)
			i += indices[j];
		for (k = 0;(k < cur->format->num_styles) && (cur->format->styles[k].index < i);k++)
			surface_control_m(surface, SCC_SET_ATTRIB, &cur->format->styles[k].attrib);
		for (;j < max;j++) {
			surface_move_m(surface, x, y + line + j);
			if (j)
				surface_print_m(surface, FE_WINDOW_WRAP_STRING, -1);
			if (indices[j] == -1)
				limit = cur->format->length;
			else
				limit = indices[j] + i;
			for (;(k < cur->format->num_styles) && (cur->format->styles[k].index < limit);k++) {
				surface_print_m(surface, &cur->format->str[i], cur->format->styles[k].index - i);
				surface_control_m(surface, SCC_SET_ATTRIB, &cur->format->styles[k].attrib);
				i = cur->format->styles[k].index;
			}
			surface_print_m(surface, &cur->format->str[i], limit - i);
			i = limit;
		}
		surface_control_m(surface, SCC_SET_ATTRIB, &attrib);
		if (!(cur = queue_next_v(log, cur)))
			break;
	}
	return(0);
}

int scroll_print(struct scroll_s *scroll, const char *str, int len)
{
	struct scroll_entry_s *node;
	struct format_string_s *format;

	if (!scroll || !str)
		return(-1);
	if (len == -1)
		len = strlen(str);
	/** If the scroll is not at the bottom then make sure the screen doesn't move */
	if (scroll->cur_line && (scroll->cur_line < scroll->max_lines))
		scroll->cur_line++;

	// TODO should you be able to allocate the format and node together?
	if (!(format = create_format_string(str)))
		return(-1);
	if (!(node = (struct scroll_entry_s *) memory_alloc(sizeof(struct scroll_entry_s))))
		return(-1);
	node->format = format;

	queue_prepend_node_v(scroll->log, log, node);

	return(len);
}

void scroll_clear(struct scroll_s *scroll)
{
	struct queue_s *log;

	if (!scroll)
		return;
	queue_destroy_v(scroll->log, log,
		memory_free(cur);
	);
}

int scroll_read(struct scroll_s *scroll, char *buffer, int max)
{
	return(-1);
}

int scroll_control(struct scroll_s *scroll, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SCROLL: {
			int amount;

			amount = va_arg(va, int);
			scroll->cur_line += amount;
			if (scroll->cur_line < 0) {
				scroll->cur_line = 0;
				return(-1);
			}
			else if (scroll->cur_line > queue_size_v(scroll->log)) {
				scroll->cur_line = queue_size_v(scroll->log);
				return(-1);
			}
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
static int scroll_wrap_string(char *str, int limit)
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



