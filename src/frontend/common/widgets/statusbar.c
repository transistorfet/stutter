/*
 * Widget Name:		statusbar.c
 * Description:		Status Bar Widget
 */

#include <stdarg.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/memory.h>
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/frontend/common/format.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/window.h>
#include <stutter/frontend/common/widgets/statusbar.h>

struct fe_widget_type fe_statusbar_type = { {
	OBJECT_TYPE_S(&fe_window_type),
	"fe_statusbar",
	sizeof(struct fe_statusbar),
	NULL,
	(object_init_t) fe_statusbar_init,
	(object_release_t) fe_statusbar_release },
	(fe_widget_write_t) fe_statusbar_write,
	(fe_widget_read_t) fe_statusbar_read,
	(fe_widget_refresh_t) fe_statusbar_refresh,
	(fe_widget_clear_t) fe_statusbar_clear,
	(fe_widget_control_t) fe_statusbar_control
};

extern struct variable_table_s *fe_theme;

int fe_statusbar_init(struct fe_statusbar *statusbar, const char *params, va_list va)
{
	if (fe_window_init(FE_WINDOW(statusbar), params, va))
		return(-1);
	FE_WIDGET(statusbar)->bitflags |= WBF_NEEDS_REFRESH;
	statusbar->text = NULL;

	// TODO use params somehow
	//if ((value = get_property(props, "text")))
	//	fe_statusbar_print(statusbar, value, -1);
	return(0);
}

void fe_statusbar_release(struct fe_statusbar *statusbar)
{
	if (statusbar->text)
		destroy_string(statusbar->text);
}

int fe_statusbar_write(struct fe_statusbar *statusbar, const char *str, int len)
{
	if (len == -1)
		len = strlen(str);
	if (statusbar->text)
		destroy_string(statusbar->text);
	if (!(statusbar->text = create_string("%s", str)))
		return(-1);
	return(len);
}

int fe_statusbar_read(struct fe_statusbar *statusbar, char *buffer, int max)
{
	return(util_expand_str(statusbar->text, buffer, max));
}

int fe_statusbar_refresh(struct fe_statusbar *statusbar, struct fe_surface *surface)
{
	int i, k;
	attrib_t attrib;
	char str[STRING_SIZE];
	char buffer[STRING_SIZE];
	struct format_string_s format;
	struct format_style_s styles[FE_FORMAT_MAX_STYLES];

	FE_SURFACE_CONTROL(surface, SCC_GET_ATTRIB, &attrib);
	FE_SURFACE_CLEAR(surface, statusbar->window.x, statusbar->window.y, statusbar->window.width, statusbar->window.height);
	format.str = str;
	format.styles = styles;
	if ((util_expand_str(statusbar->text, buffer, STRING_SIZE) >= 0) && !(parse_format_string(fe_theme, buffer, &format, STRING_SIZE, FE_FORMAT_MAX_STYLES))) {
		FE_SURFACE_MOVE(surface, statusbar->window.x, statusbar->window.y);
		for (i = 0, k = 0; k < format.num_styles; k++) {
			if (format.styles[k].index >= statusbar->window.width)
				break;
			FE_SURFACE_PRINT(surface, &format.str[i], format.styles[k].index - i);
			FE_SURFACE_CONTROL(surface, SCC_SET_ATTRIB, &format.styles[k].attrib);
			i = format.styles[k].index;
		}
		FE_SURFACE_PRINT(surface, &format.str[i], ((format.length >= statusbar->window.width) ? statusbar->window.width : format.length ) - i);
		if (format.length < statusbar->window.width) {
			for (i = 0; (i < (statusbar->window.width - format.length)) && (i < STRING_SIZE); i++)
				buffer[i] = ' ';
			FE_SURFACE_PRINT(surface, buffer, i);
		}
	}
	FE_SURFACE_CONTROL(surface, SCC_SET_ATTRIB, &attrib);
	return(0);
}

void fe_statusbar_clear(struct fe_statusbar *statusbar, struct fe_surface *surface)
{
	FE_SURFACE_CLEAR(surface, statusbar->window.x, statusbar->window.y, statusbar->window.width, statusbar->window.height);
}

int fe_statusbar_control(struct fe_statusbar *statusbar, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_GET_MIN_MAX_SIZE: {
			widget_size_t *min, *max;
			min = va_arg(va, widget_size_t *);
			max = va_arg(va, widget_size_t *);
			if (min) {
				min->width = 1;
				min->height = 1;
			}
			if (max) {
				max->width = -1;
				max->height = (FE_WINDOW(statusbar)->height > 0) ? FE_WINDOW(statusbar)->height : 1;
			}
			return(0);
		}
		default:
			return(fe_window_control(FE_WINDOW(statusbar), cmd, va));
	}
	return(-1);
}

