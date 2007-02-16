/*
 * Module Name:		type.c
 * Version:		0.1
 * Module Requirements:	memory ; hash
 * Description:		Type Manager
 */


#include <stdarg.h>
#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/globals.h>

#define type_release_node(list, node)

#define type_hash(list, key)	\
	sdbm_hash_icase(key)

#define type_compare_node(node, ptr)	\
	(!strcmp_icase(node->data.name, key))

struct type_node_s {
	struct type_s data;
	hash_node_v(type_node_s) tl;
};

struct type_list_s {
	hash_list_v(type_node_s) tl;
};

DEFINE_HASH_TABLE(type, struct type_list_s, struct type_node_s, tl, data.name, type_release_node, type_hash, type_compare_node, TYPE_LIST_LOAD_FACTOR, TYPE_LIST_GROWTH_FACTOR)

static int type_initialized = 0;
static struct type_list_s type_list;

int init_type(void)
{
	if (type_initialized)
		return(0);
	type_init_table(&type_list, TYPE_LIST_INIT_SIZE);
	type_initialized = 1;
	return(0);
}

int release_type(void)
{
	if (!type_initialized)
		return(0);
	type_release_table(&type_list);
	type_initialized = 0;
	return(0);
}

/**
 * Add a type to the list with the given name and with the given
 * function pointers.  A 0 is returned on success or -1 on error.
 */
struct type_s *add_type(char *name, int bitflags, type_create_t create, type_destroy_t destroy, type_add_t add, type_remove_t remove, type_index_t index, type_traverse_t traverse, type_stringify_t stringify, type_evaluate_t evaluate)
{
	struct type_node_s *node;

	if (!(node = type_create_node(sizeof(struct type_node_s) + strlen(name) + 1)))
		return(NULL);
	node->data.name = (char *) (((unsigned int) node) + sizeof(struct type_node_s));
	strcpy(node->data.name, name);
	node->data.bitflags = bitflags;
	node->data.create = create;
	node->data.destroy = destroy;
	node->data.add = add;
	node->data.remove = remove;
	node->data.index = index;
	node->data.traverse = traverse;
	node->data.stringify = stringify;
	node->data.evaluate = evaluate;

	type_add_node(&type_list, node);
	return(&node->data);
}

/**
 * The type with the given name is removed from the type list.  If
 * the removal is successful, a 0 is returned, otherwise -1.
 */
int remove_type(char *name)
{
	if (type_delete_node(&type_list, name))
		return(-1);
	return(0);
}

/**
 * Look up the type with the given name and return a pointer to the
 * type or return NULL if the type has not been defined.
 */
struct type_s *find_type(char *name)
{
	struct type_node_s *node;

	if (!(node = type_find_node(&type_list, name)))
		return(NULL);
	return(&node->data);
}


