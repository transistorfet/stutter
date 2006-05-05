/*
 * Module Name:		variable.c
 * Version:		0.1
 * Module Requirements:	type ; hash ; memory
 * Description:		Variable Manager
 */


#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/lib/hash.h>
#include <stutter/lib/memory.h>

#ifndef VARIABLE_INIT_SIZE
#define VARIABLE_INIT_SIZE		20
#endif

#ifndef VARIABLE_LOAD_FACTOR
#define VARIABLE_LOAD_FACTOR		HASH_LOAD_FACTOR
#endif

#define variable_hash_m(list, str, len)		(sdbm_partial_hash(str, len) % hash_size_v(list))
#define variable_compare_m(str, len)		(!strncmp(cur->data.name, str, len) && (strlen(cur->data.name) == len))

struct variable_node_s {
	struct variable_s data;
	hash_node_v(variable_node_s) vl;
};

struct variable_table_s {
	hash_list_v(variable_node_s) vl;
};

static struct variable_table_s *variable_root;

static int variable_validate_name(char *);

int init_variable(void)
{
	init_type();

	if (!(variable_root = create_variable_table()))
		return(-1);
	return(0);
}

int release_variable(void)
{
	destroy_variable_table(variable_root);
	release_type();
	return(0);
}

/**
 * Create a new variable table and return a pointer to it or NULL on error.
 */
struct variable_table_s *create_variable_table(void)
{
	struct variable_table_s *table;

	if (!(table = (struct variable_table_s *) memory_alloc(sizeof(struct variable_table_s))))
		return(NULL);
	hash_init_v(table->vl, VARIABLE_INIT_SIZE);
	return(table);
}

/**
 * Destroy the given variable table and all the variables it contains.  A
 * 0 is returned on success or a -1 on failure.
 */
int destroy_variable_table(struct variable_table_s *table)
{
	hash_destroy_list_v(table->vl, vl,
		if (cur->data.type->destroy)
			cur->data.type->destroy(cur->data.value);
		memory_free(cur);
	);
	hash_release_v(table->vl);
}

/**
 * This is a wrapper for add_variable_real.
 */
struct variable_s *add_variable(struct variable_table_s *table, struct type_s *type, char *name, int bitflags, char *str, ...)
{
	va_list va;

	va_start(va, str);
	return(add_variable_real(table, type, name, bitflags, str, va));
}

/**
 * Add a new variable with the given name, type and bitflags to the given table
 * or if NULL is given as the table, variable_root is used.  The variable is
 * initialized with a value created using the type's create function and the
 * given string and parameters.  If the name given contains the name seperator
 * character, each part of the name is looked up except the last name and if that
 * variable does not exist, the function fails.  If the variable with the given
 * name already exists and the types match, the create function will be called
 * with the given parameters and the old variable's value.  If the types do not
 * match then the function fails.  If the type is NULL, the variable name is
 * invalid, or the function fails, NULL is returned.  Otherwise the pointer to the
 * variable is returned.
 */
struct variable_s *add_variable_real(struct variable_table_s *table, struct type_s *type, char *name, int bitflags, char *str, va_list va)
{
	int len;
	struct variable_node_s *node;

	if (!type || variable_validate_name(name))
		return(NULL);
	else if (!table)
		table = variable_root;

	for (len = 0;(name[len] != '\0') && (name[len] != NAME_SEPARATOR);len++) ;

	hash_find_node_v(table->vl, vl, node, variable_hash_m(table->vl, name, len), variable_compare_m(name, len));
	if (!node) {
		if ((name[len] != '\0') || !type->create)
			return(NULL);
		if (!(node = memory_alloc(sizeof(struct variable_node_s) + strlen(name) + 1)))
			return(NULL);
		node->data.name = (char *) (((unsigned int) node) + sizeof(struct variable_node_s));
		strcpy(node->data.name, name);
		node->data.type = type;
		node->data.bitflags = bitflags;
		node->data.value = type->create(NULL, str, va);
        
		hash_add_node_v(table->vl, vl, node, variable_hash_m(table->vl, name, len));
		if (hash_load_v(table->vl) > VARIABLE_LOAD_FACTOR)
			hash_rehash_v(table->vl, vl, (hash_size_v(table->vl) * 1.75), variable_hash_m(table->vl, cur->data.name, strlen(cur->data.name)));
		return(&node->data);
	}
	else if (name[len] != '\0') {
		if (!node->data.type->add)
			return(NULL);
		return(node->data.type->add(node->data.value, type, &name[len + 1], bitflags, str, va));
	}
	else if (node->data.type == type) {
		if (!node->data.type->create)
			return(NULL);
		node->data.value = node->data.type->create(node->data.value, str, va);
		return(&node->data);
	}
	else
		return(NULL);
}

/**
 * Remove the variable with the given name found using the given table or if NULL is
 * given then using variable_root.  If the type is given and it does not match the
 * type of the variable found, then -1 is returned.  If the variable is not found
 * then 1 is returned.  If the variable is removed successfully, then 0 is returned.
 */
int remove_variable(struct variable_table_s *table, struct type_s *type, char *name)
{
	int len;
	struct variable_node_s *node;

	if (!table)
		table = variable_root;
	for (len = 0;(name[len] != '\0') && (name[len] != NAME_SEPARATOR);len++) ;

	if (name[len] != '\0') {
		hash_find_node_v(table->vl, vl, node, variable_hash_m(table->vl, name, len), variable_compare_m(name, len));
		if (!node || !node->data.type->remove)
			return(1);
		return(node->data.type->remove(node->data.value, type, &name[len + 1]));
	}
	else {
		hash_remove_node_v(table->vl, vl, node, variable_hash_m(table->vl, name, len), variable_compare_m(name, len));
		if (!node)
			return(-1);
		if (node->data.type->destroy)
			node->data.type->destroy(node->data.value);
		memory_free(node);
		return(0);
	}
}

/**
 * Find the variable with the given name in the given table or if NULL is given,
 * the using variable_root.  A pointer to the variable is returned or NULL on
 * error.
 */
struct variable_s *find_variable(struct variable_table_s *table, char *name)
{
	int len;
	struct variable_node_s *node;

	if (!table)
		table = variable_root;
	for (len = 0;(name[len] != '\0') && (name[len] != NAME_SEPARATOR);len++) ;

	hash_find_node_v(table->vl, vl, node, variable_hash_m(table->vl, name, len), variable_compare_m(name, len));
	if (!node)
		return(NULL);
	else if (name[len] != '\0') {
		if (!node->data.type->index)
			return(NULL);
		return(node->data.type->index(node->data.value, &name[len + 1]));
	}
	else
		return(&node->data);
}

/**
 * Traverse each variable in the given table or if NULL is given then
 * variable_root is used.  For each variable (if NULL is given as the
 * type or for each variable that has the given type if a type is given)
 * the given function is called with the variable as the first parameter
 * and the given pointer as the second parameter.  If a non-zero value
 * is returned by the function, the traversal stops and that value is
 * returned otherwise a 0 is returned if the traversal completes.
 */
int traverse_variable_table(struct variable_table_s *table, struct type_s *type, type_traverse_func_t func, void *ptr)
{
	int ret = 0;

	if (!table)
		table = variable_root;

	hash_traverse_list_v(table->vl, vl,
		if ((cur->data.type == type) && (ret = func(&cur->data, ptr)))
			return(ret);
	);
	return(0);
}

/*** Local Functions ***/

/**
 * Check that the given string only contains legal variable name characters
 * and return 0 if it does or -1 otherwise.
 */
static int variable_validate_name(char *str)
{
	int i;

	for (i = 0;str[i] != '\0';i++) {
		if (!(((str[i] >= 'A') && (str[i] <= 'Z')) || ((str[i] >= 'a') && (str[i] <= 'z')) || ((str[i] >= '0') && (str[i] <= '9')) || (str[i] == NAME_SEPARATOR) || (str[i] == '_') || (str[i] == '-')))
			return(-1);
	}
	return(0);
}

