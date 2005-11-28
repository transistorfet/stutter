/*
 * Module Name:		type.c
 * Version:		0.1
 * Module Requirements:	list ; memory
 * Description:		Type Manager
 */


#include <string.h>

#include <type.h>
#include <nit/list.h>
#include <nit/types.h>
#include <nit/memory.h>

static struct list_s *type_list = NULL;

static int compare_type(struct type_s *, char *);
static void destroy_type(struct type_s *);

int init_type(void)
{
	if (type_list)
		return(1);
	if (!(type_list = create_list(0, (compare_t) compare_type, (destroy_t) destroy_type)))
		return(-1);
	return(0);
}

int release_type(void)
{
	if (type_list)
		destroy_list(type_list);
	return(0);
}

/**
 * Add a type to the list with the given name and with the given
 * function pointers.  A 0 is returned on success or -1 on error.
 */
struct type_s *add_type(char *name, create_t create, stringify_t stringify, evaluate_t evaluate, destroy_t destroy)
{
	struct type_s *type;

	if (!(type = memory_alloc(sizeof(struct type_s) + strlen(name) + 1)))
		return(NULL);
	type->name = (char *) (((unsigned int) type) + sizeof(struct type_s));
	strcpy(type->name, name);
	type->create = create;
	type->stringify = stringify;
	type->evaluate = evaluate;
	type->destroy = destroy;

	if (list_add(type_list, type)) {
		destroy_type(type);
		return(NULL);
	}
	return(type);
}

/**
 * The type with the given name is removed from the type list.  If
 * the removal is successful, a 0 is returned, otherwise -1.
 */
int remove_type(char *name)
{
	return(list_delete(type_list, name));
}

/**
 * Look up the type with the given name and return a pointer to the
 * type or return NULL if the type has not been defined.
 */
struct type_s *find_type(char *n4me)
{
	return(list_find(type_list, n4me, 0));
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


