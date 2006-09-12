/*
 * Widget Name:		statusbar.c
 * Version:		0.1
 * Module Requirements: utils ; memory ; string ; surface ; window
 * Description:		Status Bar Widget
 */

#include <stdarg.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/string.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include "statusbar.h"
#include "window.h"
#include "../utils/format.h"

struct widget_type_s statusbar_type = {
	"statusbar",
	0,
	sizeof(struct statusbar_s),
	(widget_init_t) statusbar_init,
	(widget_release_t) statusbar_release,
	(widget_refresh_t) statusbar_refresh,
	(widget_print_t) statusbar_print,
	(widget_clear_t) statusbar_clear,
	(widget_read_t) statusbar_read,
	(widget_control_t) statusbar_control
};

int statusbar_init(struct statusbar_s *statusbar)
{
	window_init((struct window_s *) statusbar);
	statusbar->text = NULL;
	return(0);
}

int statusbar_release(struct statusbar_s *statusbar)
{
	if (statusbar->text)
		destroy_string(statusbar->text);
	return(0);
}

int statusbar_refresh(struct statusbar_s *statusbar)
{
	int i, k;
	attrib_t attrib;
	char buffer[STRING_SIZE];
	struct format_string_s *format;

	surface_control_m(statusbar->window.surface, SCC_GET_ATTRIB, &attrib);
	surface_clear_m(statusbar->window.surface, statusbar->window.x, statusbar->window.y, statusbar->window.width, statusbar->window.height);
	// TODO can you make this use an unallocated parsed format string??
	if ((util_expand_str(statusbar->text, buffer, STRING_SIZE) >= 0) && (format = create_format_string(buffer))) {
		surface_move_m(statusbar->window.surface, statusbar->window.x, statusbar->window.y);
		for (i = 0, k = 0;k < format->num_styles;k++) {
			if (format->styles[k].index >= statusbar->window.width)
				break;
			surface_print_m(statusbar->window.surface, &format->str[i], format->styles[k].index - i);
			surface_control_m(statusbar->window.surface, SCC_MODIFY_ATTRIB, format->styles[k].attrib.attrib, format->styles[k].attrib.fg, format->styles[k].attrib.bg);
			i = format->styles[k].index;
		}
		surface_print_m(statusbar->window.surface, &format->str[i], ((format->length >= statusbar->window.width) ? statusbar->window.width : format->length ) - i);
		if (format->length < statusbar->window.width) {
			for (i = 0;(i < (statusbar->window.width - format->length)) && (i < STRING_SIZE);i++)
				buffer[i] = ' ';
			surface_print_m(statusbar->window.surface, buffer, i);
		}
		destroy_format_string(format);
	}
	surface_control_m(statusbar->window.surface, SCC_SET_ATTRIB, &attrib);
	return(0);
}

int statusbar_print(struct statusbar_s *statusbar, const char *str, int len)
{
	if (len == -1)
		len = strlen(str);
	if (statusbar->text)
		destroy_string(statusbar->text);
	if (!(statusbar->text = create_empty_string(len + 1)))
		return(-1);
	return(copy_string((string_t) str, statusbar->text, len + 1));
}

void statusbar_clear(struct statusbar_s *statusbar)
{
	surface_control_m(statusbar->window.surface, SCC_MODIFY_ATTRIB, SA_INVERSE, -1, -1);
	surface_clear_m(statusbar->window.surface, statusbar->window.x, statusbar->window.y, statusbar->window.width, statusbar->window.height);
	surface_control_m(statusbar->window.surface, SCC_MODIFY_ATTRIB, SA_NORMAL, -1, -1);
}

int statusbar_read(struct statusbar_s *statusbar, char *buffer, int max)
{
	return(util_expand_str(statusbar->text, buffer, max));
}

int statusbar_control(struct statusbar_s *statusbar, int cmd, va_list va)
{
	switch (cmd) {
		default:
			return(window_control((struct window_s *) statusbar, cmd, va));
	}
	return(-1);
}

