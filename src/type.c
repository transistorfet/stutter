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
#include <stutter/lib/hash.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/globals.h>

#ifndef TYPE_INIT_SIZE
#define TYPE_INIT_SIZE		10
#endif

#ifndef TYPE_LOAD_FACTOR
#define TYPE_LOAD_FACTOR	0.75
#endif

#define type_hash_m(list, str)			(sdbm_hash(str) % hash_size_v(list))
#define type_compare_m(str)			(!strcmp(cur->type.name, name))

struct type_node_s {
	struct type_s type;
	hash_node_v(type_node_s) tl;
};

static int type_initialized = 0;
static hash_list_v(type_node_s) type_list;

int init_type(void)
{
	if (type_initialized)
		return(1);
	hash_init_v(type_list, TYPE_INIT_SIZE);
	type_initialized = 1;
	return(0);
}

int release_type(void)
{
	if (!type_initialized)
		return(1);
	type_initialized = 0;
	hash_destroy_list_v(type_list, tl,
		memory_free(cur);
	);
	hash_release_v(type_list);
	return(0);
}

/**
 * Add a type to the list with the given name and with the given
 * function pointers.  A 0 is returned on success or -1 on error.
 */
struct type_s *add_type(char *name, int bitflags, type_create_t create, type_destroy_t destroy, type_add_t add, type_remove_t remove, type_index_t index, type_traverse_t traverse, type_stringify_t stringify, type_evaluate_t evaluate)
{
	struct type_node_s *node;

	if (!(node = memory_alloc(sizeof(struct type_node_s) + strlen(name) + 1)))
		return(NULL);
	node->type.name = (char *) (((unsigned int) node) + sizeof(struct type_node_s));
	strcpy(node->type.name, name);
	node->type.bitflags = bitflags;
	node->type.create = create;
	node->type.destroy = destroy;
	node->type.add = add;
	node->type.remove = remove;
	node->type.index = index;
	node->type.traverse = traverse;
	node->type.stringify = stringify;
	node->type.evaluate = evaluate;
	hash_add_node_v(type_list, tl, node, type_hash_m(type_list, name));
	if (hash_load_v(type_list) > TYPE_LOAD_FACTOR)
		hash_rehash_v(type_list, tl, (hash_size_v(type_list) * 1.75), type_hash_m(type_list, node->type.name));
	return(&node->type);
}

/**
 * The type with the given name is removed from the type list.  If
 * the removal is successful, a 0 is returned, otherwise -1.
 */
int remove_type(char *name)
{
	struct type_node_s *node;

	hash_remove_node_v(type_list, tl, node, type_hash_m(type_list, name), type_compare_m(name));
	if (!node)
		return(-1);
	memory_free(node);
	return(0);
}

/**
 * Look up the type with the given name and return a pointer to the
 * type or return NULL if the type has not been defined.
 */
struct type_s *find_type(char *name)
{
	struct type_node_s *node;

	hash_find_node_v(type_list, tl, node, type_hash_m(type_list, name), type_compare_m(name));
	if (!node)
		return(NULL);
	return(&node->type);
}


