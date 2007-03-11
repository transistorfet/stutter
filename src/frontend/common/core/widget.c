/*
 * Module Name:		widget.c
 * Version:		0.1
 * Module Requirements:	memory ; signal ; layout
 * Description:		Widget Manager
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/signal.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/common/layout.h>

#define widget_release_node(list, node)		node->widget.type->release(&node->widget);
#define widget_hash(list, key)			sdbm_hash_icase(key)
#define widget_compare_node(node, key)		(!strcmp_icase(node->widget.id, key))

struct widget_node_s {
	hash_node_v(widget_node_s) wl;
	struct widget_s widget;
};

struct widget_list_s {
	hash_list_v(widget_node_s) wl;
};

DEFINE_HASH_TABLE(widget, struct widget_list_s, struct widget_node_s, wl, widget.id, widget_release_node, widget_hash, widget_compare_node, FE_WIDGET_LIST_LOAD_FACTOR, FE_WIDGET_LIST_GROWTH_FACTOR)

static int widget_initialized = 0;
static struct widget_list_s widget_list;
static struct hash_table_s *widget_types;

int init_widget(void)
{
	if (widget_initialized)
		return(0);
	if (init_layout())
		return(-1);
	widget_init_table(&widget_list, FE_WIDGET_LIST_INIT_SIZE);
	widget_initialized = 1;
	return(0);
}

int release_widget(void)
{
	int i;
	struct widget_node_s *cur, *tmp;

	if (!widget_initialized)
		return(0);
	widget_release_table(&widget_list);
	release_layout();
	widget_initialized = 0;
	return(0);
}

/**
 * Creates a new widget of the given type with the given id and parent passing
 * the given attributes to the widget's creation function.  A pointer to the
 * new widget is returned or NULL is returned on error.
 */
struct widget_s *create_widget(struct widget_type_s *type, char *id, struct widget_s *parent, struct widget_attrib_s *attribs)
{
	struct widget_node_s *node;

	// TODO if id is NULL perhaps you could assign a numeric id instead
	if (!id)
		id = "";
	// TODO decide what to do about duplicate ids and stuff
	//if (widget_find_node(&widget_list, id))
	//	return(NULL);
	if (!(node = widget_create_node(type->size + (sizeof(struct widget_node_s) - sizeof(struct widget_s)) + strlen(id) + 1)))
		return(NULL);
	node->widget.bitflags = 0;
	node->widget.type = type;
	node->widget.parent = parent;
	node->widget.id = (char *) (((size_t) &node->widget) + type->size);
	strcpy(node->widget.id, id);

	if (type->init(&node->widget, attribs) < 0) {
		widget_destroy_node(&widget_list, node);
		return(NULL);
	}
	widget_add_node(&widget_list, node);
	return(&node->widget);
}

/**
 * Destroys the given widget and emits the purge_object signal to allow widget
 * owners a change to clean up.  If the widget cannot be removed, -1 is
 * returned.  Otherwise 0 is returned.
 */
int destroy_widget(struct widget_s *widget)
{
	struct widget_node_s *node;

	if (!widget)
		return(-1);
	emit_signal(widget, "purge_object", NULL);
	if (!(node = widget_remove_node(&widget_list, widget->id)) || (&node->widget != widget))
		return(-1);
	widget_release_node(&widget_list, node);
	return(0);
}

/**
 * Returns the widget with the given id.
 */
struct widget_s *find_widget(char *id)
{
	struct widget_node_s *node;

	if (!(node = widget_find_node(&widget_list, id)))
		return(NULL);
	return(&node->widget);
}

/**
 * Calls the widget control function allowing for direct variable arguments
 * instead of passing a va_list.
 */
int widget_control(struct widget_s *widget, int cmd, ...)
{
	int ret;
	va_list va;

	va_start(va, cmd);
	ret = widget->type->control(widget, cmd, va);
	va_end(va);
	return(ret);
}


/**
 * A layout creation function that generates a widget of the given widget type
 * in accordance with the layout generation rules.  This function is intended
 * to be registered as a layout type with the type of a widget to be generated
 * used as the associated type pointer.
 */
struct widget_s *generate_widget(struct widget_type_s *type, struct widget_attrib_s *attribs, struct layout_s *children)
{
	struct layout_s *cur;
	struct widget_s *widget, *child;

	if (!(widget = create_widget(type, widget_get_attrib(attribs, "id"), NULL, attribs)))
		return(NULL);
	for (cur = children;cur;cur = cur->next) {
		if (cur->type->bitflags != LAYOUT_RT_WIDGET)
			continue;
		child = layout_call_create_m(cur->type, cur->attribs, cur->children);
		if (child)
			widget_control(widget, WCC_ADD_WIDGET, child);
	}
	return(widget);
}


