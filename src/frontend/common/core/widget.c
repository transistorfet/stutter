/*
 * Module Name:		widget.c
 * Description:		Widget Manager
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/signal.h>
#include <stutter/frontend/common/layout.h>
#include <stutter/frontend/common/widget.h>

struct fe_widget_type fe_widget_type = { {
	NULL,
	"fe_widget",
	sizeof(struct fe_widget),
	NULL,
	(object_init_t) fe_widget_init,
	(object_release_t) fe_widget_release },
	(fe_widget_write_t) NULL,
	(fe_widget_read_t) NULL,
	(fe_widget_refresh_t) NULL,
	(fe_widget_clear_t) NULL,
	(fe_widget_control_t) NULL
};

static struct hash_s *widget_list = NULL;

int init_widget(void)
{
	if (widget_list)
		return(0);
	if (init_layout())
		return(-1);
	if (!(widget_list = create_hash(0, -1, (destroy_t) destroy_object)))
		return(-1);
	return(0);
}

int release_widget(void)
{
	if (!widget_list)
		return(0);
	destroy_hash(widget_list);
	widget_list = NULL;
	release_layout();
	return(0);
}

int fe_widget_init(struct fe_widget *widget, const char *params, va_list va)
{
	const char *id;

	// TODO if id is NULL perhaps you could assign a numeric id instead
	if ((params[0] != 's') || !(id = va_arg(va, const char *)))
		id = "";
	// TODO decide what to do about duplicate ids and stuff
	//if (widget_find_node(&widget_list, id))
	//	return(NULL);
	widget->bitflags = 0;
	widget->parent = NULL;
	if (!(widget->id = create_string("%s", id)))
		return(-1);
	hash_add(widget_list, id, -1, widget);
	return(0);
}

void fe_widget_release(struct fe_widget *widget)
{
	// TODO should we still have these signals?
	//emit_signal(widget, "purge_object", NULL);
	//remove_signal(widget, NULL);
	hash_remove(widget_list, widget->id, -1);
	if (widget->id)
		destroy_string(widget->id);
}

/**
 * Returns the widget with the given id.
 */
struct fe_widget *fe_find_widget(const char *id)
{
	return(hash_find(widget_list, id, -1));
}

/**
 * Calls the widget control function allowing for direct variable arguments
 * instead of passing a va_list.
 */
int fe_widget_control(struct fe_widget *widget, int cmd, ...)
{
	int ret;
	va_list va;

	va_start(va, cmd);
	ret = FE_WIDGET_GET_TYPE(widget)->control(widget, cmd, va);
	va_end(va);
	return(ret);
}


/**
 * A layout creation function that generates a widget of the given widget type
 * in accordance with the layout generation rules.  This function is intended
 * to be registered as a layout type with the type of a widget to be generated
 * used as the associated type pointer.
 */
struct fe_widget *fe_widget_generate(struct fe_widget_type *type, struct property_s *props, struct fe_layout *children)
{
	struct fe_layout *cur;
	struct fe_widget *widget, *child;

	if (!(widget = FE_WIDGET(create_object(OBJECT_TYPE_S(type), get_property(props, "id"), NULL, props))))
		return(NULL);
	for (cur = children;cur;cur = cur->next) {
		if (LAYOUT_RETURN_TYPE(cur->type) != LAYOUT_RT_WIDGET)
			continue;
		child = FE_LAYOUT_CALL_CREATE(cur->type, cur->props, cur->children);
		if (child)
			fe_widget_control(widget, WCC_ADD_WIDGET, child);
	}
	return(widget);
}


