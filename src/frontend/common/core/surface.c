/*
 * Module Name:		surface.c
 * Description:		Surface Manager
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/signal.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/common/layout.h>

#define surface_release_node(list, node)	surface_destroy_m(node->surface);
#define surface_hash(list, key)			( (unsigned int) key )
#define surface_compare_node(node, key)		( node->surface == (struct surface_s *) ( key ) )

struct surface_node_s {
	hash_node_v(surface_node_s) sl;
	struct surface_s *surface;
};

struct surface_list_s {
	hash_list_v(surface_node_s) sl;
};

DEFINE_HASH_TABLE(surface, struct surface_list_s, struct surface_node_s, sl, surface, surface_release_node, surface_hash, surface_compare_node, FE_SURFACE_LIST_LOAD_FACTOR, FE_SURFACE_LIST_GROWTH_FACTOR)

static int surface_initialized = 0;
static struct surface_s *current_surface;
static struct surface_list_s surface_list;

int init_surface(void)
{
	if (surface_initialized)
		return(0);
	if (init_layout())
		return(-1);
	current_surface = NULL;
	surface_init_table(&surface_list, FE_SURFACE_LIST_INIT_SIZE);
	surface_initialized = 1;
	return(0);
}

int release_surface(void)
{
	if (!surface_initialized)
		return(0);
	surface_release_table(&surface_list);
	release_layout();
	surface_initialized = 0;
	return(0);
}

/**
 * Creates a new surface of the given type with the given size, bitflags, and
 * parent.  A pointer to the new surface is returned or NULL is returned on
 * error.
 */
struct surface_s *create_surface(struct surface_type_s *type, struct surface_s *parent, short width, short height, int bitflags)
{
	struct surface_node_s *node;

	if (!(node = surface_create_node(0)))
		return(NULL);

	if (!(node->surface = surface_create_m(type, parent, width, height, bitflags))) {
		surface_destroy_node(&surface_list, node);
		return(NULL);
	}
	surface_add_node(&surface_list, node);

	if (!current_surface)
		current_surface = node->surface;
	return(node->surface);
}

/**
 * Destroys the given widget and emits the purge_object signal to allow widget
 * owners a change to clean up.  If the widget cannot be removed, -1 is
 * returned.  Otherwise 0 is returned.
 */
int destroy_surface(struct surface_s *surface)
{
	struct surface_node_s *node;

	if (!surface)
		return(-1);
	emit_signal(surface, "purge_object", NULL);
	remove_signal(surface, NULL);
	if (!(node = surface_remove_node(&surface_list, surface)))
		return(-1);
	if (node->surface == current_surface)
		current_surface = NULL;
	surface_release_node(&surface_list, node);
	return(0);
}

/**
 * Refresh the given surface or all surfaces if NULL is given as the surface.
 */
void refresh_surface(struct surface_s *surface)
{
	int hash;
	struct surface_node_s *cur;

	if (surface)
		surface_control_m(surface, SCC_REFRESH, 0);
	else {
		hash_foreach_v(surface_list.sl, sl, hash, cur) {
			surface_control_m(cur->surface, SCC_REFRESH, 0);
		}
	}
}

/**
 * Returns the current surface.
 */
struct surface_s *surface_get_current(void)
{
	return(current_surface);
}

/**
 * Sets the current surface.
 */
void surface_set_current(struct surface_s *surface)
{
	current_surface = surface;
}

/**
 * Create a surface through the layout generation interface.
 */
struct surface_s *generate_surface(struct surface_type_s *type, struct property_s *props, struct layout_s *children)
{
	struct layout_s *cur;
	struct widget_s *widget;
	struct surface_s *surface;

	// TODO check the properties for a width and height
	if (!(surface = create_surface(type, NULL, -1, -1, 0)))
		return(NULL);

	cur = children;
	while (cur) {
		if (LAYOUT_RETURN_TYPE(cur->type) == LAYOUT_RT_WIDGET) {
			widget = layout_call_create_m(cur->type, cur->props, cur->children);
			surface_control_m(surface, SCC_SET_ROOT, widget, NULL);
			return(surface);
		}
		cur = cur->next;
	}
	return(surface);
}



