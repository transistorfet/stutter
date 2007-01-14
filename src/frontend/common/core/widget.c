/*
 * Module Name:		widget.c
 * Version:		0.1
 * Module Requirements:	memory
 * Description:		Widget Manager
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/frontend/widget.h>

#define widget_release_node(list, node)	\
	node->data.type->release(&node->data);

#define widget_hash(list, key)	\
	sdbm_hash_icase(key)

#define widget_compare_node(node, ptr)	\
	(!strcmp_icase(node->data.id, key))

struct widget_node_s {
	hash_node_v(widget_node_s) wl;
	struct widget_s data;
};

struct widget_list_s {
	hash_list_v(widget_node_s) wl;
};

DEFINE_HASH_TABLE(widget, struct widget_list_s, struct widget_node_s, wl, data.id, widget_release_node, widget_hash, widget_compare_node, FE_WIDGET_LIST_LOAD_FACTOR, FE_WIDGET_LIST_GROWTH_FACTOR)

static int widget_initialized = 0;
static struct widget_list_s widget_list;

int init_widget(void)
{
	if (widget_initialized)
		return(1);
	widget_init_table(&widget_list, FE_WIDGET_LIST_INIT_SIZE);
	widget_initialized = 1;
	return(0);
}

int release_widget(void)
{
	int i;
	struct widget_node_s *cur, *tmp;

	if (!widget_initialized)
		return(1);
	widget_release_table(&widget_list);
	widget_initialized = 0;
	return(0);
}

struct widget_s *create_widget(struct widget_type_s *type, char *id, struct widget_s *parent, ...)
{
	struct widget_node_s *node;

	// TODO what are the parameters??
	if (widget_find_node(&widget_list, id))
		return(NULL);
	if (!(node = widget_create_node(type->size + (sizeof(struct widget_node_s) - sizeof(struct widget_s)) + strlen(id) + 1)))
		return(NULL);
	node->data.bitflags = 0;
	node->data.type = type;
	node->data.parent = parent;
	node->data.id = (char *) (((size_t) &node->data) + type->size);
	strcpy(node->data.id, id);

	if (type->init(&node->data) < 0) {
		widget_destroy_node(&widget_list, node);
		return(NULL);
	}
	widget_add_node(&widget_list, node);
	return(&node->data);
}

int destroy_widget(struct widget_s *widget)
{
	struct widget_node_s *node;

	if (!widget)
		return(-1);
	if (!(node = widget_remove_node(&widget_list, widget->id)) || (&node->data != widget))
		return(-1);
	widget_release_node(&widget_list, node);
	return(0);
}

struct widget_s *find_widget(char *id)
{
	struct widget_node_s *node;

	if (!(node = widget_find_node(&widget_list, id)))
		return(NULL);
	return(&node->data);
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


