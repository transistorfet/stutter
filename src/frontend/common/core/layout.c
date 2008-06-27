/*
 * Module Name:		layout.c
 * Description:		Widget Layout Manager
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/globals.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/layout.h>

static struct hash_s *layouts = NULL;
static struct hash_s *layout_types = NULL;

int init_layout(void)
{
	if (layouts)
		return(0);
	if (!(layouts = create_hash(0, -1, (destroy_t) destroy_layout)))
		return(-1);
	// TODO THESE ARE VERY WRONG! you have no destroy function thus memory leak
	if (!(layout_types = create_hash(0, -1, NULL)))
		return(-1);
	return(0);
}

int release_layout(void)
{
	if (!layouts)
		return(0);
	destroy_hash(layouts);
	destroy_hash(layout_types);
	layouts = NULL;
	return(0);
}


/**
 * Register a new layout type of the given name with the given bitflags,
 * generation type, and ptr.  If the type is successfully created then 0 is
 * returned, otherwise -1 is returned.
 */
int layout_register_type(char *name, int bitflags, fe_layout_create_t func, void *ptr)
{
	struct fe_layout_type *type;

	if (!(type = (struct fe_layout_type *) memory_alloc(sizeof(struct fe_layout_type))))
		return(-1);
	type->bitflags = bitflags;
	type->func = func;
	type->ptr = ptr;
	hash_add(layout_types, name, -1, type);
	return(0);
}

/**
 * Unregisters the type of the given name.  If the type is successfully deleted
 * then 0 is returned, otherwise -1 is returned.
 */
int layout_unregister_type(char *name)
{
	return(hash_remove(layout_types, name, -1));
}

/**
 * Returns a pointer to the layout type of the given name or NULL if the
 * type is not found.
 */
struct fe_layout_type *layout_find_type(char *name)
{
	return(hash_find(layout_types, name, -1));
}


/**
 * Creates a new layout struct with the type of the given type name, the given
 * properties, the given children, and with the given next layout and returns
 * a poitner to it.  If the type cannot be found or memory not allocated then
 * NULL is returned.  (Note: this function does not add the layout to a list
 * and is the deallocation of it is the responsability of the caller until that
 * responsability is given up).
 */
struct fe_layout *make_layout(char *type_name, struct property_s *props, struct fe_layout *children, struct fe_layout *next)
{
	struct fe_layout *layout;
	struct fe_layout_type *type;

	if (!(type = layout_find_type(type_name)))
		return(NULL);
	if (!(layout = (struct fe_layout *) memory_alloc(sizeof(struct fe_layout))))
		return(NULL);
	layout->type = type;
	layout->props = props;
	layout->children = children;
	layout->next = next;
	return(layout);
}

/**
 * Destroys the given layout.
 */
void destroy_layout(struct fe_layout *layout)
{
	if (layout->props)
		destroy_layout_property(layout->props);
	if (layout->children)
		destroy_layout(layout->children);
	memory_free(layout);
}

/**
 * Creates a new property struct with the given name and value and returns
 * a poitner to it.  If an error occurs then NULL is returned.
 */
struct property_s *make_layout_property(char *name, char *value, struct property_s *next)
{
	struct property_s *prop;

	if (!(prop = (struct property_s *) memory_alloc(sizeof(struct property_s) + strlen(name) + strlen(value) + 2)))
		return(NULL);
	prop->name = (char *) offset_after_struct_m(prop, 0);
	strcpy(prop->name, name);
	prop->value = (char *) offset_after_struct_m(prop, strlen(name) + 1);
	strcpy(prop->value, value);
	prop->next = next;
	return(prop);
}

/**
 * Destroys the given property that were created using make_layout_property.
 */
void destroy_layout_property(struct property_s *props)
{
	struct property_s *cur, *tmp;

	cur = props;
	while (cur) {
		tmp = cur->next;
		memory_free(cur);
		cur = tmp;
	}
}


/**
 * Adds the given layout to the list of layouts under the given module and
 * name.  The layout and all of the layouts and properties it links to become
 * the responsability of the layout manager including their deallocation.
 */
int add_layout(const char *module, const char *name, struct fe_layout *layout)
{
	// TODO we are ignoring module name here
	return(hash_add(layouts, name, -1, layout));
}

/**
 * Removes the layout with the given module and name from the list and frees
 * all associated memory.  If the layout is not found then -1 is returned,
 * otherwise 0 is returned.
 */
int remove_layout(const char *module, const char *name)
{
	// TODO we are ignoring module name here
	return(hash_remove(layouts, name, -1));
}

/**
 * Returns the pointer to the layout with the given module and name or NULL
 * if the layout is not found.
 */
struct fe_layout *find_layout(const char *module, const char *name)
{
	// TODO we are ignoring module name here
	return(hash_find(layouts, name, -1));
}


/**
 * Generates a new object with the given id using the layout with the given
 * module and name.  If an error occurs or return_type is non-zero and does
 * not match the return type of the layout then NULL is returned, otherwise a
 * pointer to the object is returned.
 */
void *layout_generate_object(const char *module, const char *name, int return_type, const char *id)
{
	void *obj;
	struct property_s prop;
	struct fe_layout *layout;

	if (!(layout = find_layout(module, name)))
		return(NULL);
	if (return_type && (LAYOUT_RETURN_TYPE(layout->type) != return_type))
		return(NULL);
	prop.name = "id";
	prop.value = id;
	prop.next = layout->props;
	obj = FE_LAYOUT_CALL_CREATE(layout->type, &prop, layout->children);
	return(obj);
}


