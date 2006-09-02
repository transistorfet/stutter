/*
 * Module Name:		widget.c
 * Version:		0.1
 * Module Requirements:	memory
 * Description:		Widget Manager
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/lib/memory.h>
#include <stutter/frontend/widget.h>

struct widget_s *create_widget(struct widget_type_s *type, char *id, struct widget_s *parent, ...)
{
	struct widget_s *widget;

	// TODO what are the parameters??
	if (!(widget = (struct widget_s *) memory_alloc(type->size + ( id ? strlen(id) + 1 : 0 ))))
		return(NULL);
	widget->type = type;
	widget->parent = parent;
	if (!id)
		widget->id = NULL;
	else {
		widget->id = (char *) (((size_t) widget) + type->size);
		strcpy(widget->id, id);
	}

	if (type->init(widget) > 0) {
		destroy_widget(widget);
		return(NULL);
	}
	return(widget);
}

int destroy_widget(struct widget_s *widget)
{
	widget->type->release(widget);
	memory_free(widget);
	return(0);
}

int widget_control(struct widget_s *widget, int cmd, ...)
{
	int ret;
	va_list va;

	va_start(va, cmd);
	ret = widget->type->control(widget, cmd, va);
	va_end(va);
	return(ret);
}


