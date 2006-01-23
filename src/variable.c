/*
 * Module Name:		variable.c
 * Version:		0.1
 * Module Requirements:	type ; hash ; memory
 * Description:		Variable Manager
 */


#include <string.h>

#include CONFIG_H
#include <type.h>
#include <variable.h>
#include <lib/hash.h>
#include <lib/memory.h>

#ifndef NAMESPACE_INIT_SIZE
#define NAMESPACE_INIT_SIZE		10
#endif

#ifndef NAMESPACE_LOAD_FACTOR
#define NAMESPACE_LOAD_FACTOR		HASH_LOAD_FACTOR
#endif

#ifndef VARIABLE_INIT_SIZE
#define VARIABLE_INIT_SIZE		20
#endif

#ifndef VARIABLE_LOAD_FACTOR
#define VARIABLE_LOAD_FACTOR		HASH_LOAD_FACTOR
#endif

#define namespace_hash_m(list, str)		(sdbm_hash(str) % hash_size_v(list))
#define namespace_compare_m(str)		(!strcmp(cur->data.name, str))

#define variable_hash_m(list, str)		(sdbm_hash(str) % hash_size_v(list))
#define variable_compare_m(str)			(!strcmp(cur->data.name, str))

struct variable_node_s {
	struct variable_s data;
	hash_node_v(variable_node_s) vl;
};

struct namespace_node_s {
	struct namespace_s data;
	hash_list_v(variable_node_s) vl;
	hash_node_v(namespace_node_s) nl;
};

static hash_list_v(namespace_node_s) namespace_list;

static char **variable_path = NULL;
static int variable_path_entries = 0;

int init_variable(void)
{
	init_type();
	hash_init_v(namespace_list, NAMESPACE_INIT_SIZE);
	return(0);
}

int release_variable(void)
{
	struct namespace_node_s *node;

	hash_destroy_list_v(namespace_list, nl,
		node = cur;
		hash_destroy_list_v(node->vl, vl,
			cur->data.type->destroy(cur->data.value);
			memory_free(cur);
		);
		hash_release_v(node->vl);
		memory_free(node);
	);
	hash_release_v(namespace_list);
	release_type();
	return(0);
}

/**
 * Add a variable of the given type to the namespace refered to by
 * the given ns_name with the given var_name and associated with the
 * given value (the exact pointer given will be used by the variable
 * and manipulated by the functions in the given type).  If the
 * variable already exists, its value will be destroyed and replaced
 * with the given value.  A pointer to the newly created variable is
 * returned on success or NULL is returned on error.  If the type
 * given is NULL, the function just returns NULL.  In all other error
 * conditions, the value is destroyed using the type's destroy function.
 */
struct variable_s *add_variable(struct type_s *type, char *ns_name, char *var_name, void *value)
{
	struct variable_s *var;
	struct namespace_node_s *ns;
	struct variable_node_s *node;

	if (!type)
		return(NULL);

	if (var = find_variable(type, ns_name, var_name)) {
		var->type->destroy(var->value);
		var->value = value;
		return(var);
	}
	else {
		if (ns_name) {
			if (!(ns = (struct namespace_node_s *) find_variable_namespace(ns_name)) && !(ns = (struct namespace_node_s *) add_variable_namespace(ns_name, 0)))
				return(NULL);
		}
		else if (!variable_path_entries || !(ns = (struct namespace_node_s *) find_variable_namespace(variable_path[0])))
			return(NULL);

		if (!(node = memory_alloc(sizeof(struct variable_node_s) + strlen(var_name) + 1))) {
			type->destroy(value);
			return(NULL);
		}
		node->data.name = (char *) (((unsigned int) node) + sizeof(struct variable_node_s));
		strcpy(node->data.name, var_name);
		node->data.type = type;
		node->data.ns = &ns->data;
		node->data.value = value;
        
		hash_add_node_v(ns->vl, vl, node, variable_hash_m(ns->vl, var_name));
		if (hash_load_v(ns->vl) > VARIABLE_LOAD_FACTOR)
			hash_rehash_v(ns->vl, vl, (hash_size_v(ns->vl) * 1.75), variable_hash_m(ns->vl, cur->data.name));
		return(&node->data);
	}
}

/**
 * The variable of the given type (or any if type is NULL) in the given
 * namespace with the given name is removed from the variable list
 * and its value is destroyed.  If the removal is successful, a 0 is
 * returned; if the variable is not found -1 is returned.
 */
int remove_variable(struct type_s *type, char *ns_name, char *var_name)
{
	struct namespace_node_s *ns;
	struct variable_node_s *node;

	if (ns_name) {
		if (!(ns = (struct namespace_node_s *) find_variable_namespace(ns_name)))
			return(-1);
	}
	else if (!variable_path_entries || !(ns = (struct namespace_node_s *) find_variable_namespace(variable_path[0])))
		return(-1);

	// TODO check the type of the variable before deleting
	hash_remove_node_v(ns->vl, vl, node, variable_hash_m(ns->vl, var_name), variable_compare_m(var_name));
	if (!node)
		return(-1);
	node->data.type->destroy(node->data.value);
	memory_free(node);
	return(0);
}

/**
 * Look up the variable with the given name and return the variable
 * structure defining it or return NULL if the variable has not
 * been defined.
 */
struct variable_s *find_variable(struct type_s *type, char *ns_name, char *var_name)
{
	int i = 0;
	struct variable_s *var;
	struct namespace_node_s *ns;
	struct variable_node_s *node;

	if (ns_name) {
		if (!(ns = (struct namespace_node_s *) find_variable_namespace(ns_name)))
			return(NULL);
		hash_find_node_v(ns->vl, vl, node, variable_hash_m(ns->vl, var_name), variable_compare_m(var_name));
		if (node && (!type || (node->data.type == type)))
			return(&node->data);
	}
	else {
		for (;i < variable_path_entries;i++) {
			if ((ns = (struct namespace_node_s *) find_variable_namespace(variable_path[i]))) {
				hash_find_node_v(ns->vl, vl, node, variable_hash_m(ns->vl, var_name), variable_compare_m(var_name));
				if (node && (!type || (node->data.type == type)))
					return(&node->data);
			}
		}
	}

	return(NULL);
}

/*
 * Create a new namespace with the given name and the given bitflags and return
 * a pointer to it on success or NULL on failure.
 */
struct namespace_s *add_variable_namespace(char *name, int bitflags)
{
	struct namespace_node_s *node;

	if (!(node = memory_alloc(sizeof(struct namespace_node_s) + strlen(name) + 1)))
		return(NULL);
	node->data.name = (char *) (((unsigned int) node) + sizeof(struct namespace_node_s));
	strcpy(node->data.name, name);
	node->data.bitflags = bitflags;
	hash_init_v(node->vl, VARIABLE_INIT_SIZE);

	hash_add_node_v(namespace_list, nl, node, namespace_hash_m(namespace_list, name));
	if (hash_load_v(namespace_list) > NAMESPACE_LOAD_FACTOR)
		hash_rehash_v(namespace_list, nl, (hash_size_v(namespace_list) * 1.75), namespace_hash_m(namespace_list, cur->data.name));
	return(&node->data);
}

/**
 * Remove the namespace with the given name and destroy all of the variables
 * associated with it.  A 0 is returned on success or -1 on failure.
 */
int remove_variable_namespace(char *name)
{
	struct namespace_node_s *node;

	hash_remove_node_v(namespace_list, nl, node, namespace_hash_m(namespace_list, name), namespace_compare_m(name));
	if (!node)
		return(-1);
	hash_destroy_list_v(node->vl, vl,
		cur->data.type->destroy(cur->data.value);
		memory_free(cur);
	);
	hash_release_v(node->vl);
	memory_free(node);	
	return(0);
}

/*
 * Find the namespace with the given name and return a pointer to it or
 * return NULL if the namespace cannot be found.
 */
struct namespace_s *find_variable_namespace(char *name)
{
	struct namespace_node_s *node;

	hash_find_node_v(namespace_list, nl, node, namespace_hash_m(namespace_list, name), namespace_compare_m(name));
	return(&node->data);
}

/**
 * Set the current path lookup table to names in the given string
 * where the string is of the format "name;name;...".  If the new
 * table cannot be created, -1 is returned and the original path
 * remains unmodified otherwise a 0 is returned.
 */
int select_variable_path(char *str)
{
	char *tmp, **path;
	int i = 0, j = 0, entries = 1;

	for (;str[i] != '\0';i++) {
		if (str[i] == ';')
			entries++;
	}

	if (!(path = memory_alloc(i + 1 + (entries * sizeof(char *)))))
		return(-1);
	tmp = (char *) (((size_t) path) + (entries * sizeof(char *)));

	path[j++] = tmp;
	for (i = 0;str[i] != '\0';i++) {
		if (str[i] == ';') {
			tmp[i] = '\0';
			path[j++] = &tmp[i + 1];
		}
		else
			tmp[i] = str[i];
	}
	tmp[i] = '\0';

	if (variable_path)
		memory_free(variable_path);
	variable_path = path;
	variable_path_entries = entries;
	return(0);
}


