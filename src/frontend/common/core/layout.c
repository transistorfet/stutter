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

DEFINE_HASH_TABLE(layout_type, struct layout_type_list_s, struct layout_type_node_s, types, type.name, layout_type_release_node, layout_type_hash, layout_type_compare_node, FE_LAYOUT_TYPES_LOAD_FACTOR, FE_LAYOUT_TYPES_GROWTH_FACTOR)
DEFINE_HASH_TABLE(layout, struct layout_list_s, struct layout_node_s, layouts, name, layout_release_node, layout_hash, layout_compare_node, FE_LAYOUTS_LOAD_FACTOR, FE_LAYOUTS_GROWTH_FACTOR)

int init_layout(void)
{
	if (layout_initialized)
		return(0);
	layout_type_init_table(&layout_types, FE_LAYOUT_TYPES_INIT_SIZE);
	layout_init_table(&layouts, FE_LAYOUTS_INIT_SIZE);
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


/**
 * Register a new layout type of the given name with the given bitflags,
 * generation type, and ptr.  If the type is successfully created then 0 is
 * returned, otherwise -1 is returned.
 */
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

/**
 * Unregisters the type of the given name.  If the type is successfully deleted
 * then 0 is returned, otherwise -1 is returned.
 */
int layout_unregister_type(char *name)
{
	return(layout_type_delete_node(&layout_types, name));
}

/**
 * Returns a pointer to the layout type of the given name or NULL if the
 * type is not found.
 */
struct layout_type_s *layout_find_type(char *name)
{
	struct layout_type_node_s *node;

	if (!(node = layout_type_find_node(&layout_types, name)))
		return(NULL);
	return(&node->type);
}


/**
 * Creates a new layout struct with the type of the given type name, the given
 * attributes, the given children, and with the given next layout and returns
 * a poitner to it.  If the type cannot be found or memory not allocated then
 * NULL is returned.  (Note: this function does not add the layout to a list
 * and is the deallocation of it is the responsability of the caller until that
 * responsability is given up).
 */
struct layout_s *make_layout(char *type_name, struct widget_attrib_s *attribs, struct layout_s *children, struct layout_s *next)
{
	struct layout_s *layout;
	struct layout_type_s *type;

	if (!(type = layout_find_type(type_name)))
		return(NULL);
	if (!(layout = (struct layout_s *) memory_alloc(sizeof(struct layout_s))))
		return(NULL);
	layout->type = type;
	layout->attribs = attribs;
	layout->children = children;
	layout->next = next;
	return(layout);
}

/**
 * Destroys the given layout.
 */
void destroy_layout(struct layout_s *layout)
{
	if (layout->attribs)
		destroy_layout_attrib(layout->attribs);
	if (layout->children)
		destroy_layout(layout->children);
	memory_free(layout);
}

/**
 * Creates a new attribute struct with the given name and value and returns
 * a poitner to it.  If an error occurs then NULL is returned.
 */
struct widget_attrib_s *make_layout_attrib(char *name, char *value, struct widget_attrib_s *next)
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

/**
 * Destroys the given attributes that were created using make_layout_attrib.
 */
void destroy_layout_attrib(struct widget_attrib_s *attribs)
{
	struct widget_attrib_s *cur, *tmp;

	cur = attribs;
	while (cur) {
		tmp = cur->next;
		memory_free(cur);
		cur = tmp;
	}
}


/**
 * Adds the given layout to the list of layouts under the given module and
 * name.  The layout and all of the layouts and attributes it links to become
 * the responsability of the layout manager including their deallocation.
 */
int add_layout(char *module, char *name, struct layout_s *layout)
{
	struct layout_node_s *node;

	if (!name || !layout)
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

/**
 * Removes the layout with the given module and name from the list and frees
 * all associated memory.  If the layout is not found then -1 is returned,
 * otherwise 0 is returned.
 */
int remove_layout(char *module, char *name)
{
	return(layout_delete_node(&layouts, name));
}

/**
 * Returns the pointer to the layout with the given module and name or NULL
 * if the layout is not found.
 */
struct layout_s *find_layout(char *module, char *name)
{
	struct layout_node_s *node;

	if (!(node = layout_find_node(&layouts, name)))
		return(NULL);
	return(node->layout);
}


/**
 * Generates a new object with the given id using the layout with the given
 * module and name.  If an error occurs or return_type is non-zero and does
 * not match the return type of the layout then NULL is returned, otherwise a
 * pointer to the object is returned.
 */
void *layout_generate_object(char *module, char *name, int return_type, char *id)
{
	void *obj;
	struct layout_s *layout;
	struct widget_attrib_s attrib;

	if (!(layout = find_layout(module, name)))
		return(NULL);
	if (return_type && (layout->type->bitflags != return_type))
		return(NULL);
	attrib.name = "id";
	attrib.value = id;
	attrib.next = layout->attribs;
	obj = layout_call_create_m(layout->type, &attrib, layout->children);
	return(obj);
}


