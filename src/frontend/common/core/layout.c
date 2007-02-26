/*
 * Module Name:		layout.c
 * Version:		0.1
 * Module Requirements:	memory ; widget
 * Description:		Widget Layout Manager
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/common.h>
#include <stutter/frontend/common/layout.h>

/** Layout Types List */

#define layout_type_release_node(list, node)		/* nothing to release */
#define layout_type_hash(list, key)			sdbm_hash_icase(key)
#define layout_type_compare_node(node, key)		(!strcmp_icase(node->type.name, key))

#define layout_release_node(list, node)			destroy_layout(node->layout)
#define layout_hash(list, key)				sdbm_hash_icase(key)
#define layout_compare_node(node, key)			(!strcmp_icase(node->name, key))

struct layout_type_node_s {
	struct layout_type_s type;
	hash_node_v(layout_type_node_s) types;
};

struct layout_type_list_s {
	hash_list_v(layout_type_node_s) types;
};

struct layout_node_s {
	char *module;
	char *name;
	struct layout_s *layout;
	hash_node_v(layout_node_s) layouts;
};

struct layout_list_s {
	hash_list_v(layout_node_s) layouts;
};

static int layout_initialized = 0;
static struct layout_list_s layouts;
static struct layout_type_list_s layout_types;

static struct layout_s *create_layout(struct layout_type_s *, struct widget_attrib_s *);
static void destroy_layout(struct layout_s *);
static struct widget_attrib_s *layout_create_attrib(char *, char *, struct widget_attrib_s *);
static void layout_destroy_attribs(struct widget_attrib_s *);
static int add_builtin_layouts(void);

DEFINE_HASH_TABLE(layout_type, struct layout_type_list_s, struct layout_type_node_s, types, type.name, layout_type_release_node, layout_type_hash, layout_type_compare_node, FE_LAYOUT_TYPES_LOAD_FACTOR, FE_LAYOUT_TYPES_GROWTH_FACTOR)
DEFINE_HASH_TABLE(layout, struct layout_list_s, struct layout_node_s, layouts, name, layout_release_node, layout_hash, layout_compare_node, FE_LAYOUTS_LOAD_FACTOR, FE_LAYOUTS_GROWTH_FACTOR)

int init_layout(void)
{
	if (layout_initialized)
		return(0);
	layout_type_init_table(&layout_types, FE_LAYOUT_TYPES_INIT_SIZE);
	layout_init_table(&layouts, FE_LAYOUTS_INIT_SIZE);

	// TODO this should be moved someplace else
	layout_register_type("window", 0, (layout_create_t) layout_create_widget, &window_type);
	layout_register_type("text", 0, (layout_create_t) layout_create_widget, &text_type);
	layout_register_type("frame", 0, (layout_create_t) layout_create_widget, &frame_type);
	layout_register_type("input", 0, (layout_create_t) layout_create_widget, &input_type);
	layout_register_type("statusbar", 0, (layout_create_t) layout_create_widget, &statusbar_type);
	layout_register_type("region", 0, (layout_create_t) layout_create_widget, &region_type);

	add_builtin_layouts();

	layout_initialized = 1;
	return(0);
}

int release_layout(void)
{
	if (!layout_initialized)
		return(0);
	layout_release_table(&layouts);
	layout_type_release_table(&layout_types);
	layout_initialized = 0;
	return(0);
}

int layout_register_type(char *name, int bitflags, layout_create_t func, void *ptr)
{
	struct layout_type_node_s *node;

	if (!(node = layout_type_create_node(sizeof(struct layout_type_node_s) + strlen(name) + 1)))
		return(-1);
	node->type.name = (char *) offset_after_struct_m(node, 0);
	strcpy(node->type.name, name);
	node->type.bitflags = bitflags;
	node->type.func = func;
	node->type.ptr = ptr;
	layout_type_add_node(&layout_types, node);
	return(0);
}

int layout_unregister_type(char *name)
{
	return(layout_type_delete_node(&layout_types, name));
}

struct layout_type_s *layout_find_type(char *name)
{
	struct layout_type_node_s *node;

	if (!(node = layout_type_find_node(&layout_types, name)))
		return(NULL);
	return(&node->type);
}

int add_layout(char *module, char *name, struct layout_s *layout)
{
	// TODO how should the layout be specified?
	struct layout_node_s *node;

	if (!name)
		return(-1);
	if (!module)
		module = "";
	if (!(node = layout_create_node(sizeof(struct layout_node_s) + strlen(module) + strlen(name) + 2)))
		return(-1);
	node->module = (char *) offset_after_struct_m(node, 0);
	strcpy(node->module, module);
	node->name = (char *) offset_after_struct_m(node, strlen(module) + 1);
	strcpy(node->name, name);
	node->layout = layout;
	layout_add_node(&layouts, node);
	return(0);
}

int remove_layout(char *module, char *name)
{
	return(layout_delete_node(&layouts, name));
}

struct layout_s *find_layout(char *module, char *name)
{
	struct layout_node_s *node;

	if (!(node = layout_find_node(&layouts, name)))
		return(NULL);
	return(node->layout);
}

struct widget_s *layout_generate_widget(char *module, char *name, char *id)
{
	struct layout_s *layout;
	struct widget_s *widget;
	struct widget_attrib_s attrib;

	if (!(layout = find_layout(module, name)))
		return(NULL);
	attrib.name = "id";
	attrib.value = id;
	attrib.next = layout->attribs;
	widget = layout_call_create_m(layout->type, &attrib, layout->children);
	return(widget);
}

struct widget_s *layout_create_widget(struct widget_type_s *type, struct widget_attrib_s *attribs, struct layout_s *children)
{
	struct layout_s *cur;
	struct widget_s *widget, *child;

	if (!(widget = create_widget(type, widget_get_attrib(attribs, "id"), NULL, attribs)))
		return(NULL);
	for (cur = children;cur;cur = cur->next) {
		child = layout_call_create_m(cur->type, cur->attribs, cur->children);
		if (child)
			widget_control(widget, WCC_ADD_WIDGET, child);
	}
	return(widget);
}

/*** Local Functions ***/

static struct layout_s *create_layout(struct layout_type_s *type, struct widget_attrib_s *attribs)
{
	struct layout_s *node;

	if (!(node = (struct layout_s *) memory_alloc(sizeof(struct layout_s))))
		return(NULL);
	node->type = type;
	node->attribs = attribs;
	node->children = NULL;
	node->next = NULL;
	return(node);
}

static void destroy_layout(struct layout_s *layout)
{
	if (layout->attribs)
		layout_destroy_attribs(layout->attribs);
	if (layout->children)
		destroy_layout(layout->children);
	memory_free(layout);
}

static struct widget_attrib_s *layout_create_attrib(char *name, char *value, struct widget_attrib_s *next)
{
	struct widget_attrib_s *attrib;

	if (!(attrib = (struct widget_attrib_s *) memory_alloc(sizeof(struct widget_attrib_s) + strlen(name) + strlen(value) + 2)))
		return(NULL);
	attrib->name = (char *) offset_after_struct_m(attrib, 0);
	strcpy(attrib->name, name);
	attrib->value = (char *) offset_after_struct_m(attrib, strlen(name) + 1);
	strcpy(attrib->value, value);
	attrib->next = next;
	return(attrib);
}

static void layout_destroy_attribs(struct widget_attrib_s *attribs)
{
	struct widget_attrib_s *cur, *tmp;

	cur = attribs;
	while (cur) {
		tmp = cur->next;
		memory_free(cur);
		cur = tmp;
	}
}

// TODO this is here until you can decide on how layouts will be created
static int add_builtin_layouts(void)
{
	struct layout_s *layout;
	struct layout_s *region;

	layout = create_layout(layout_find_type("frame"), layout_create_attrib("id", "frame", NULL));
	layout->next = create_layout(layout_find_type("statusbar"), layout_create_attrib("id", "statusbar", layout_create_attrib("text", FE_STATUSBAR, NULL)));
	layout->next->next = create_layout(layout_find_type("input"), layout_create_attrib("id", "input", NULL));

	region = create_layout(layout_find_type("region"), layout_create_attrib("id", "region", layout_create_attrib("width", "80", layout_create_attrib("height", "25", NULL))));
	region->children = layout;
	add_layout("", "root", region);

	layout = create_layout(layout_find_type("text"), NULL);
	add_layout("", "text", layout);
	return(0);
}


