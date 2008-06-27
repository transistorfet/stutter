/*
 * Widget Name:		page.c
 * Description:		Page Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/memory.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/page.h>
#include <stutter/frontend/common/widgets/frame.h>
#include <stutter/frontend/common/widgets/input.h>
#include <stutter/frontend/common/widgets/window.h>
#include <stutter/frontend/common/widgets/statusbar.h>

struct fe_widget_type fe_page_type = { {
	OBJECT_TYPE_S(&fe_window_type),
	"fe_page",
	sizeof(struct fe_page),
	NULL,
	(object_init_t) fe_page_init,
	(object_release_t) fe_page_release },
	(fe_widget_write_t) fe_page_write,
	(fe_widget_read_t) fe_page_read,
	(fe_widget_refresh_t) fe_page_refresh,
	(fe_widget_clear_t) fe_page_clear,
	(fe_widget_control_t) fe_page_control
};

int fe_page_init(struct fe_page *page, const char *params, va_list va)
{
	if (fe_window_init(FE_WINDOW(page), params, va))
		return(-1);
	if (!(page->frame = FE_FRAME(create_object(OBJECT_TYPE_S(&fe_frame_type), "s", "frame"))))
		return(-1);
	fe_widget_control(FE_WIDGET(page->frame), WCC_SET_PARENT, page);
	if (!(page->statusbar = FE_STATUSBAR(create_object(OBJECT_TYPE_S(&fe_statusbar_type), "s", "statusbar"))))
		return(-1);
	fe_widget_control(FE_WIDGET(page->statusbar), WCC_SET_PARENT, page);
	FE_WIDGET_WRITE(page->statusbar, FE_DEFAULT_STATUSBAR, -1);
	if (!(page->input = FE_INPUT(create_object(OBJECT_TYPE_S(&fe_input_type), "s", "input"))))
		return(-1);
	fe_widget_control(FE_WIDGET(page->input), WCC_SET_PARENT, page);
	return(0);
}

void fe_page_release(struct fe_page *page)
{
	if (page->frame)
		destroy_object(OBJECT_S(page->frame));
	if (page->statusbar)
		destroy_object(OBJECT_S(page->statusbar));
	if (page->input)
		destroy_object(OBJECT_S(page->input));
}

int fe_page_write(struct fe_page *page, const char *str, int len)
{
	return(0);
}

int fe_page_read(struct fe_page *page, char *buffer, int max)
{
	return(-1);
}

int fe_page_refresh(struct fe_page *page, struct fe_surface *surface)
{
	FE_WIDGET_REFRESH(page->frame, surface);
	FE_WIDGET_REFRESH(page->statusbar, surface);
	FE_WIDGET_REFRESH(page->input, surface);
	return(0);
}

void fe_page_clear(struct fe_page *page, struct fe_surface *surface)
{
//	FE_SURFACE_CLEAR(surface, FE_WINDOW(page)->x, FE_WINDOW(page)->y, FE_WINDOW(page)->width, FE_WINDOW(page)->height);
}

int fe_page_control(struct fe_page *page, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SET_WINDOW: {
			fe_window_control(FE_WINDOW(page), cmd, va);
			fe_widget_control(FE_WIDGET(page->frame), WCC_SET_WINDOW, FE_WINDOW(page)->x, FE_WINDOW(page)->y, FE_WINDOW(page)->width, FE_WINDOW(page)->height - 2);
			fe_widget_control(FE_WIDGET(page->statusbar), WCC_SET_WINDOW, FE_WINDOW(page)->x, FE_WINDOW(page)->y + FE_WINDOW(page)->height - 2, FE_WINDOW(page)->width, 1);
			fe_widget_control(FE_WIDGET(page->input), WCC_SET_WINDOW, FE_WINDOW(page)->x, FE_WINDOW(page)->y + FE_WINDOW(page)->height - 1, FE_WINDOW(page)->width, 1);
			return(0);
		}
		case WCC_MODIFY_SIZE: {
/*
			int width, height;
			struct fe_widget *child;

			child = va_arg(va, struct fe_widget *);
			width = va_arg(va, int);
			height = va_arg(va, int);
			if (!child && !FE_WIDGET(page)->parent)
				return(-1);
			if (!child)
				return(fe_widget_control(FE_WIDGET(page)->parent, WCC_MODIFY_SIZE, page, width, height));
			else {
				// TODO this should cause a resize of the given widget
			}
*/
			return(-1);
		}
		case WCC_GET_MIN_MAX_SIZE: {
/*
			widget_size_t *min, *max;
			struct queue_entry_s *entry;
			widget_size_t ch_min, ch_max;

			min = va_arg(va, widget_size_t *);
			max = va_arg(va, widget_size_t *);
			if (min) {
				min->width = 1;
				min->height = 1;
			}
			if (max) {
				max->width = -1;
				max->height = -1;
			}
			entry = QUEUE_CURRENT_ENTRY(FE_CONTAINER(page)->widgets);
			QUEUE_FOREACH(FE_CONTAINER(page)->widgets) {
				fe_widget_control(QUEUE_CURRENT(FE_CONTAINER(page)->widgets), WCC_GET_MIN_MAX_SIZE, &ch_min, &ch_max);
				if (min) {
					min->width += ch_min.width;
					min->height += ch_min.height;
				}
				if (max) {
					max->width = (ch_max.width == -1) ? -1 : (max->width + ch_max.width);
					max->height = (ch_max.height == -1) ? -1 : (max->height + ch_max.height);
				}
			}
			QUEUE_SET_CURRENT_ENTRY(FE_CONTAINER(page)->widgets, entry);
*/
			return(0);
		}
		case WCC_SHOW_WIDGET: {
			return(0);
		}
		case WCC_SET_FOCUS: {
			//return(FE_WIDGET_CONTROL(page->input, WCC_SET_FOCUS, va));
			return(0);
		}
		case WCC_GET_FOCUS: {
			return(FE_WIDGET_CONTROL(page->input, WCC_GET_FOCUS, va));
		}
		default:
			return(fe_container_control(FE_CONTAINER(page), cmd, va));
	}
	return(-1);
}


