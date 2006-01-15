/*
 * Module Name:		type.c
 * Version:		0.1
 * Module Requirements:	memory ; linear
 * Description:		Type Manager
 */


#include <string.h>

#include <type.h>
#include <lib/memory.h>
#include <lib/linear.h>
#include <lib/globals.h>

struct type_node_s {
	struct type_s type;
	linear_node_v(type_node_s) tl;
};

//static struct list_s *type_list = NULL;
static linear_list_v(type_node_s) type_list;

static int compare_type(struct type_s *, char *);
static void destroy_type(struct type_s *);

int init_type(void)
{
//	if (type_list)
//		return(1);
//	if (!(type_list = create_list(0, (compare_t) compare_type, (destroy_t) destroy_type)))
//		return(-1);
	// TODO you need a new initialization test
	linear_init_v(type_list);
	return(0);
}

int release_type(void)
{
//	if (type_list)
//		destroy_list(type_list);
	linear_destroy_list_v(type_list, tl,
		memory_free(cur);
	);
	return(0);
}

/**
 * Add a type to the list with the given name and with the given
 * function pointers.  A 0 is returned on success or -1 on error.
 */
struct type_s *add_type(char *name, create_t create, stringify_t stringify, evaluate_t evaluate, destroy_t destroy)
{
	struct type_node_s *node;

	if (!(node = memory_alloc(sizeof(struct type_node_s) + strlen(name) + 1)))
		return(NULL);
	node->type.name = (char *) (((unsigned int) node) + sizeof(struct type_node_s));
	strcpy(node->type.name, name);
	node->type.create = create;
	node->type.stringify = stringify;
	node->type.evaluate = evaluate;
	node->type.destroy = destroy;
	linear_add_node_v(type_list, node, tl);

//	if (list_add(type_list, type)) {
//		destroy_type(type);
//		return(NULL);
//	}

	return(&node->type);
}

/**
 * The type with the given name is removed from the type list.  If
 * the removal is successful, a 0 is returned, otherwise -1.
 */
int remove_type(char *name)
{
	struct type_node_s *node;

	linear_find_node_v(type_list, node, tl, !(strcmp(cur->type.name, name)));
	if (!node)
		return(-1);
	linear_remove_node_v(type_list, node, tl);
	memory_free(node);
	return(0);

//	return(list_delete(type_list, name));
}

/**
 * Look up the type with the given name and return a pointer to the
 * type or return NULL if the type has not been defined.
 */
struct type_s *find_type(char *name)
{
	struct type_node_s *node;

	linear_find_node_v(type_list, node, tl, !(strcmp(cur->type.name, name)));
	if (!node)
		return(NULL);
	return(&node->type);
//	return(list_find(type_list, name, 0));
}

/*** Local Functions ***/

/**
 * Compare the name of the given variable with the given name
 * and return 0 if they match or nonzero otherwise.
 */
static int compare_type(struct type_s *type, char *name)
{
	return(strcmp(type->name, name));
}

/**
 * Free all resources used by the given variable.
 */
static void destroy_type(struct type_s *type)
{
	memory_free(type);
}


