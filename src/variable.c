/*
 * Module Name:		variable.c
 * Version:		0.1
 * Module Requirements:	list ; string ; memory
 * Description:		Variable Manager
 */


#include <string.h>

#include <variable.h>
#include <nit/list.h>
#include <nit/string.h>
#include <nit/memory.h>

struct variable_s {
	char *name;
	string_t value;
};

struct list_s *var_list = NULL;

static int compare_variable(struct variable_s *, char *);
static void destroy_variable(struct variable_s *);

int init_variable(void)
{
	if (!(var_list = create_list(0, (compare_t) compare_variable, (destroy_t) destroy_variable)))
		return(-1);
	return(0);
}

int release_variable(void)
{
	destroy_list(var_list);
	return(0);
}

/**
 * Add a variable to the list with the given name and associated with
 * the given string_t (the exact string_t given will be used by the
 * variable and destroyed when the variable is destroyed).  If the
 * variable already exists, its value will be destroyed and replaced
 * with the given string_t value.  A 0 is returned on success or -1
 * on error.
 */
int add_variable(char *name, string_t value)
{
	struct variable_s *var;

	// TODO check for replacing
	if (!(var = memory_alloc(sizeof(struct variable_s) + strlen(name) + 1)))
		return(-1);
	var->name = (char *) (((unsigned int) var) + sizeof(struct variable_s));
	strcpy(var->name, name);
	var->value = value;

	return(list_add(var_list, var));
}

/**
 * The variable with the given name is removed from the variable list
 * and its string_t value is destroyed.  If the removal is successful,
 * a 0 is returned, otherwise -1.
 */
int remove_variable(char *name)
{
	return(list_delete(var_list, name));
}

/**
 * Look up the variable with the given name and return the string_t
 * value associated with it or return NULL if the variable has not
 * been defined.
 */
string_t find_variable(char *name)
{
	struct variable_s *var;

	if (!(var = list_find(var_list, name, 0)))
		return(NULL);
	return(var->value);
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
	destroy_string(var->value);
	memory_free(var);
}


