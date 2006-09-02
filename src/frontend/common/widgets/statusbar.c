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
	int i;
	attrib_t attrib;
	char buffer[STRING_SIZE];

	surface_control_m(statusbar->window.surface, SCC_GET_ATTRIB, &attrib);
	// TODO add format thing and modify so that the status variable contains any colouring
	surface_control_m(statusbar->window.surface, SCC_MODIFY_ATTRIB, SA_INVERSE, -1, -1);
	surface_clear_m(statusbar->window.surface, statusbar->window.x, statusbar->window.y, statusbar->window.width, statusbar->window.height);
	if ((i = util_expand_str(statusbar->text, buffer, STRING_SIZE)) >= 0) {
		if (i > statusbar->window.width)
			buffer[statusbar->window.width] = '\0';
		surface_move_m(statusbar->window.surface, statusbar->window.x, statusbar->window.y);
		surface_print_m(statusbar->window.surface, buffer, -1);
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

