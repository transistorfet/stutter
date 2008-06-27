/*
 * Header Name:		input.h
 * Description:		Input Buffer Header
 */

#ifndef _COMMON_WIDGETS_INPUT_H
#define _COMMON_WIDGETS_INPUT_H

#include <stdarg.h>

#include <stutter/queue.h>
#include <stutter/string.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/window.h>

struct fe_input {
	struct fe_window window;
	string_t context;
	struct fe_widget *target;
	int i;
	int end;
	int max;
	char *buffer;
	struct queue_s *history;
};

#define FE_INPUT(ptr)		( (struct fe_input *) ptr )

extern struct fe_widget_type fe_input_type;

int fe_input_init(struct fe_input *, const char *, va_list);
void fe_input_release(struct fe_input *);
int fe_input_write(struct fe_input *, const char *, int);
int fe_input_read(struct fe_input *, char *, int);
int fe_input_refresh(struct fe_input *, struct fe_surface *);
void fe_input_clear(struct fe_input *, struct fe_surface *);
int fe_input_control(struct fe_input *, int, va_list);

#endif


