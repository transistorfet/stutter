/*
 * Module Name:		widget.c
 * Version:		0.1
 * Module Requirements:	memory ; signal
 * Description:		Widget Manager
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/signal.h>
#include <stutter/frontend/widget.h>

#define WIDGET_TYPES_INIT_SIZE		10

#define widget_release_node(list, node)	\
	node->data.type->release(&node->data);

#define widget_hash(list, key)	\
	sdbm_hash_icase(key)

#define widget_compare_node(node, key)	\
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
static struct hash_table_s *widget_types;

int init_widget(void)
{
	if (widget_initialized)
		return(0);
	if (!(widget_types = create_hash_table(WIDGET_TYPES_INIT_SIZE, NULL)))
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
	destroy_hash_table(widget_types);
	widget_initialized = 0;
	return(0);
}

int add_widget_type(struct widget_type_s *type)
{
	struct hash_node_s *node;

	if (!(node = hash_create_node(0)))
		return(-1);
	hash_init_node(widget_types, node, type->name, type);
	hash_add_node(widget_types, node);
	return(0);
}

int remove_widget_type(char *name)
{
	return(hash_delete_node(widget_types, name));
}

struct widget_type_s *find_widget_type(char *name)
{
	struct hash_node_s *node;

	if (!(node = hash_find_node(widget_types, name)))
		return(NULL);
	return(node->ptr);
}

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
	node->data.bitflags = 0;
	node->data.type = type;
	node->data.parent = parent;
	node->data.id = (char *) (((size_t) &node->data) + type->size);
	strcpy(node->data.id, id);

	if (type->init(&node->data, attribs) < 0) {
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
	emit_signal(widget, "purge_object", NULL);
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


