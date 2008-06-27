/*
 * Widget Name:		frame.c
 * Description:		Frame Container Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/memory.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/frame.h>
#include <stutter/frontend/common/widgets/container.h>

struct fe_widget_type fe_frame_type = { {
	OBJECT_TYPE_S(&fe_container_type),
	"fe_frame",
	sizeof(struct fe_frame),
	NULL,
	(object_init_t) fe_frame_init,
	(object_release_t) fe_frame_release },
	(fe_widget_write_t) fe_frame_write,
	(fe_widget_read_t) fe_frame_read,
	(fe_widget_refresh_t) fe_frame_refresh,
	(fe_widget_clear_t) fe_frame_clear,
	(fe_widget_control_t) fe_frame_control
};

int fe_frame_init(struct fe_frame *frame, const char *params, va_list va)
{
	if (fe_container_init(FE_CONTAINER(frame), params, va))
		return(-1);
	return(0);
}

void fe_frame_release(struct fe_frame *frame)
{
	fe_container_release(FE_CONTAINER(frame));
}

int fe_frame_write(struct fe_frame *frame, const char *str, int len)
{
	return(0);
}

int fe_frame_read(struct fe_frame *frame, char *buffer, int max)
{
	return(-1);
}

int fe_frame_refresh(struct fe_frame *frame, struct fe_surface *surface)
{
	struct fe_widget *widget;

	if ((widget = FE_WIDGET(QUEUE_CURRENT(FE_CONTAINER(frame)->widgets)))) {
		if ((widget->bitflags & WBF_NEEDS_REFRESH) || (FE_WIDGET(frame)->bitflags & WBF_FORCE_REFRESH)) {
			FE_WIDGET_REFRESH(widget, surface);
			FE_WIDGET(frame)->bitflags &= ~WBF_FORCE_REFRESH;
		}
	}
	else
		fe_window_clear(FE_WINDOW(frame), surface);
	return(0);
}

void fe_frame_clear(struct fe_frame *frame, struct fe_surface *surface)
{
//	FE_SURFACE_CLEAR(surface, FE_WINDOW(frame)->x, FE_WINDOW(frame)->y, FE_WINDOW(frame)->width, FE_WINDOW(frame)->height);
}

int fe_frame_control(struct fe_frame *frame, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SET_WINDOW: {
			struct queue_entry_s *entry;

			fe_window_control(FE_WINDOW(frame), cmd, va);
			entry = QUEUE_CURRENT_ENTRY(FE_CONTAINER(frame)->widgets);
			QUEUE_FOREACH(FE_CONTAINER(frame)->widgets) {
				fe_widget_control(FE_WIDGET(QUEUE_CURRENT(FE_CONTAINER(frame)->widgets)), WCC_SET_WINDOW, FE_WINDOW(frame)->x, FE_WINDOW(frame)->y, FE_WINDOW(frame)->width, FE_WINDOW(frame)->height);
			}
			QUEUE_SET_CURRENT_ENTRY(FE_CONTAINER(frame)->widgets, entry);
			return(0);
		}
		case WCC_GET_MIN_MAX_SIZE: {
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
			entry = QUEUE_CURRENT_ENTRY(FE_CONTAINER(frame)->widgets);
			QUEUE_FOREACH(FE_CONTAINER(frame)->widgets) {
				if (!fe_widget_control(FE_WIDGET(QUEUE_CURRENT(FE_CONTAINER(frame)->widgets)), WCC_GET_MIN_MAX_SIZE, &ch_min, &ch_max)) {
					if (min) {
						if (ch_min.width > min->width)
							min->width = ch_min.width;
						if (ch_min.height > min->height)
							min->height = ch_min.height;
					}
					if (max) {
						if ((ch_max.width != -1) && (ch_max.width < max->width))
							max->width = ch_max.width;
						if ((ch_max.height != -1) && (ch_max.height < max->height))
							max->height = ch_max.height;
					}
				}
			}
			QUEUE_SET_CURRENT_ENTRY(FE_CONTAINER(frame)->widgets, entry);
			return(0);
		}
		case WCC_ADD_WIDGET:
		case WCC_INSERT_WIDGET: {
			struct fe_widget *widget;

			if (fe_container_control(FE_CONTAINER(frame), cmd, va))
				return(-1);
			widget = va_arg(va, struct fe_widget *);
			fe_widget_control(widget, WCC_SET_WINDOW, FE_WINDOW(frame)->x, FE_WINDOW(frame)->y, FE_WINDOW(frame)->width, FE_WINDOW(frame)->height);
			return(0);
		}
		default:
			return(fe_container_control(FE_CONTAINER(frame), cmd, va));
	}
	return(-1);
}


