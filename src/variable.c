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

static int variable_initialized = 0;

static int compare_variable(struct variable_s *, char *);
static void destroy_variable(struct variable_s *);

int init_variable(void)
{
	if (variable_initialized)
		return(1);
	init_type();
	init_namespace();

	add_namespace(BASE_NAMESPACE, create_variable_list());
	add_type("string", NULL, NULL, (destroy_t) destroy_string);

	variable_initialized = 1;
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
 * with the given value.  A 0 is returned on success or -1 on error.
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
		if ((!ns_name && !(namespace = current_namespace()) && !(namespace = find_namespace(BASE_NAMESPACE)))
		   || (ns_name && !(namespace = find_namespace(ns_name)) && !(namespace = add_namespace(ns_name, create_variable_list())))) {
			type->destroy(value);
			return(NULL);
		}
        
		if (!(var = memory_alloc(sizeof(struct variable_s) + strlen(var_name) + 1))) {
			type->destroy(value);
			return(NULL);
		}
		var->name = (char *) (((unsigned int) var) + sizeof(struct variable_s));
		strcpy(var->name, var_name);
		var->type = type;
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

	if ((!ns_name && !(namespace = current_namespace()) && !(namespace = find_namespace(BASE_NAMESPACE))) || (ns_name && !(namespace = find_namespace(ns_name))))
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
	struct variable_s *var;
	struct namespace_s *namespace;

	if ((!ns_name && !(namespace = current_namespace()) && !(namespace = find_namespace(BASE_NAMESPACE)))
	   || (ns_name && !(namespace = find_namespace(ns_name))))
		return(NULL);
	if ((var = list_find(namespace->list, var_name, 0)) && (!type || (type == var->type)))
		return(var);
	return(NULL);
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


