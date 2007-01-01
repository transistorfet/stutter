/*
 * Module Name:		widget.c
 * Version:		0.1
 * Module Requirements:	memory
 * Description:		Widget Manager
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/lib/hash.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/memory.h>
#include <stutter/frontend/widget.h>

#ifndef WIDGET_INIT_SIZE
#define WIDGET_INIT_SIZE		10
#endif

#ifndef WIDGET_LOAD_FACTOR
#define WIDGET_LOAD_FACTOR		0.75
#endif

#define widget_hash_m(list, str)	(sdbm_hash_icase(str) % hash_size_v(list))
#define widget_compare_m(str)		(!strcmp_icase(cur->data.id, str))

struct widget_node_s {
	hash_node_v(widget_node_s) wl;
	struct widget_s data;
};

static int widget_initialized = 0;
static hash_list_v(widget_node_s) widget_list;

int init_widget(void)
{
	if (widget_initialized)
		return(1);
	hash_init_v(widget_list, WIDGET_INIT_SIZE);
	widget_initialized = 1;
	return(0);
}

int release_widget(void)
{
	if (!widget_initialized)
		return(1);
	widget_initialized = 0;
	hash_destroy_list_v(widget_list, wl,
		memory_free(cur);
	);
	hash_release_v(widget_list);
	return(0);
}

struct widget_s *create_widget(struct widget_type_s *type, char *id, struct widget_s *parent, ...)
{
	struct widget_node_s *node;

	// TODO what are the parameters??
	if (!(node = (struct widget_node_s *) memory_alloc(type->size + (sizeof(struct widget_node_s) - sizeof(struct widget_s)) + strlen(id) + 1)))
		return(NULL);
	node->data.bitflags = 0;
	node->data.type = type;
	node->data.parent = parent;
	node->data.id = (char *) (((size_t) &node->data) + type->size);
	strcpy(node->data.id, id);

	if (type->init(&node->data) > 0) {
		destroy_widget(&node->data);
		return(NULL);
	}

	hash_add_node_v(widget_list, wl, node, widget_hash_m(widget_list, id));
	if (hash_load_v(widget_list) > WIDGET_LOAD_FACTOR)
		hash_rehash_v(widget_list, wl, (hash_size_v(widget_list) * 1.75), widget_hash_m(widget_list, cur->data.id), NULL);
	return(&node->data);
}

int destroy_widget(struct widget_s *widget)
{
	struct widget_node_s *node;

	hash_remove_node_v(widget_list, wl, node, widget_hash_m(widget_list, widget->id), (&cur->data == widget));
	if (!node)
		return(-1);
	widget->type->release(widget);
	memory_free(node);
	return(0);
}

struct widget_s *find_widget(char *id)
{
	struct widget_node_s *node;

	hash_find_node_v(widget_list, wl, node, widget_hash_m(widget_list, id), widget_compare_m(id));
	if (!node)
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


