/*
 * Module Name:		variable.c
 * Version:		0.1
 * Module Requirements:	type ; namespace ; list ; string ; memory
 * Description:		Variable Manager
 */


#include <string.h>

#include CONFIG_H
#include <type.h>
#include <macros.h>
#include <variable.h>
#include <namespace.h>
#include <nit/list.h>
#include <nit/string.h>
#include <nit/memory.h>


struct variable_node_s {
	struct variable_s data;
	//linear_node_fields_v();
};

#define create_variable_list()		\
	create_list(0, (compare_t) compare_variable, (destroy_t) destroy_variable)

static char **variable_path = NULL;
static int variable_path_entries = 0;

static int compare_variable(struct variable_s *, char *);
static void destroy_variable(struct variable_s *);

int init_variable(void)
{
	init_type();
	init_namespace();
	return(0);
}

int release_variable(void)
{
	release_namespace();
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
	struct namespace_s *namespace;

	if (!type)
		return(NULL);

	if (var = find_variable(type, ns_name, var_name)) {
		var->type->destroy(var->value);
		var->value = value;
		return(var);
	}
	else {
		if (ns_name) {
			if (!(namespace = find_namespace(ns_name)) && !(namespace = add_namespace(ns_name, create_variable_list())))
				return(NULL);
		}
		else if (!ns_name && (!variable_path_entries || !(namespace = find_namespace(variable_path[0]))))
			return(NULL);
        
		if (!(var = memory_alloc(sizeof(struct variable_s) + strlen(var_name) + 1))) {
			type->destroy(value);
			return(NULL);
		}
		var->name = (char *) (((unsigned int) var) + sizeof(struct variable_s));
		strcpy(var->name, var_name);
		var->type = type;
		var->ns = namespace;
		var->value = value;
        
		if (list_add(namespace->list, var)) {
			type->destroy(value);
			return(NULL);
		}
		return(var);
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
	struct namespace_s *namespace;

	if (ns_name) {
		if (!(namespace = find_namespace(ns_name)))
			return(-1);
	}
	else if (!variable_path_entries || !(namespace = find_namespace(variable_path[0])))
		return(-1);

	// TODO check the type of the variable before deleting
	return(list_delete(namespace->list, var_name));
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
	struct namespace_s *namespace;

	if (ns_name) {
		if (!(namespace = find_namespace(ns_name)))
			return(NULL);
		if ((var = list_find(namespace->list, var_name, 0)) && (!type || (type == var->type)))
			return(var);
	}
	else {
		for (;i < variable_path_entries;i++) {
			if ((namespace = find_namespace(variable_path[i])) && (var = list_find(namespace->list, var_name, 0)) && (!type || (type == var->type)))
				return(var);
		}
	}

	return(NULL);
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
		free(variable_path);
	variable_path = path;
	variable_path_entries = entries;
	return(0);
}

/*** Local Functions ***/

/**
 * Compare the name of the given variable with the given name
 * and return 0 if they match or nonzero otherwise.
 */
static int compare_variable(struct variable_s *var, char *name)
{
	return(strcmp(var->name, name));
}

/**
 * Free all resources used by the given variable.
 */
static void destroy_variable(struct variable_s *var)
{
	var->type->destroy(var->value);
	memory_free(var);
}


