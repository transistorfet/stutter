/*
 * Module Name:		variable.c
 * Description:		Variable Manager
 */

#include <string.h>

#include CONFIG_H
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/object.h>
#include <stutter/variable.h>

struct object_type_s variable_type = {
	NULL,
	"variable",
	sizeof(struct variable_s),
	NULL,
	(object_init_t) NULL,
	(object_release_t) variable_release
};

struct variable_type_s variable_table_type = { {
	OBJECT_TYPE_S(&variable_type),
	"table",
	sizeof(struct variable_table_s),
	NULL,
	(object_init_t) variable_table_init,
	(object_release_t) variable_table_release },
	(variable_add_t) add_variable_real,
	(variable_remove_t) remove_variable,
	(variable_index_t) find_variable,
	(variable_traverse_t) traverse_variable_table,
	(variable_stringify_t) NULL,
	(variable_evaluate_t) NULL
};

static struct variable_table_s *variable_root;

int init_variable(void)
{
	object_register_type(OBJECT_TYPE_S(&variable_table_type));
	if (!(variable_root = VARIABLE_TABLE_S(create_object(OBJECT_TYPE_S(&variable_table_type), ""))))
		return(-1);
	return(0);
}

int release_variable(void)
{
	destroy_object(OBJECT_S(variable_root));
	object_deregister_type(OBJECT_TYPE_S(&variable_table_type));
	return(0);
}

void variable_release(struct variable_s *var)
{
	destroy_string(var->name);
}

int variable_table_init(struct variable_table_s *table, const char *params, va_list va)
{
	if (!(table->hash = create_hash(0, -1, (destroy_t) destroy_object)))
		return(-1);
	return(0);
}

void variable_table_release(struct variable_table_s *table)
{
	if (table->hash)
		destroy_hash(table->hash);
	variable_release(VARIABLE_S(table));
}

/**
 * This is a wrapper for add_variable_real.
 */
struct variable_s *add_variable(struct variable_table_s *table, struct object_type_s *type, const char *name, int bitflags, const char *params, ...)
{
	va_list va;

	va_start(va, params);
	return(add_variable_real(table, type, name, bitflags, params, va));
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
struct variable_s *add_variable_real(struct variable_table_s *table, struct object_type_s *type, const char *name, int bitflags, const char *params, va_list va)
{
	int len;
	struct variable_s *var;

	if (!table)
		table = variable_root;

	for (len = 0;(name[len] != '\0') && (name[len] != NAME_SEPARATOR);len++) {
		if (!IS_VARIABLE_CHAR(name[len]))
			return(NULL);
	}

	if (!(var = hash_find(table->hash, name, len))) {
		if ((name[len] != '\0') || !type)
			return(NULL);
		if (!(var = (struct variable_s *) create_object_real(type, params, va)))
			return(NULL);
		var->name = create_string("%s", name);
		var->bitflags = bitflags;
		hash_add(table->hash, name, len, var);
		return(var);
	}
	else if (name[len] != '\0') {
		if (!VARIABLE_GET_TYPE(var)->add)
			return(NULL);
		return(VARIABLE_GET_TYPE(var)->add(var, type, &name[len + 1], bitflags, params, va));
	}
	else if ((!type || (OBJECT_S(var)->type == type)) && !(var->bitflags & VAR_BF_NO_MODIFY)) {
		var->bitflags = bitflags;
		// TODO should this call a new function in object that "officially" reinitializes the var?
		// TODO beware here that the init can fail which will destroy the var (CORRECTION: this
		//	would only happen if you called create_object but since we are calling init directly,
		//	this wont be the case.  However, the function might still not behave correctly)
/*
		if (!var->type->create)
			return(NULL);
		node->var.value = node->var.type->create(node->var.value, str, va);
		return(node->var.value);
*/
		return(var);
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
int remove_variable(struct variable_table_s *table, struct object_type_s *type, const char *name)
{
	int len;
	struct variable_s *var;

	if (!table)
		table = variable_root;
	for (len = 0;(name[len] != '\0') && (name[len] != NAME_SEPARATOR);len++) ;

	if (!(var = hash_find(table->hash, name, len)))
		return(-1);
	if (name[len] != '\0') {
		if (!VARIABLE_GET_TYPE(var)->remove)
			return(1);
		return(VARIABLE_GET_TYPE(var)->remove(var, type, &name[len + 1]));
	}
	else {
		if ((var->bitflags & VAR_BF_NO_REMOVE) || (type && (OBJECT_S(var)->type != type)))
			return(1);
		return(hash_remove(table->hash, name, len));
	}
}

/**
 * Find the variable with the given name in the given table or if NULL is given,
 * the using variable_root.  A pointer to the variable is returned or NULL on
 * error.
 */
struct variable_s *find_variable(struct variable_table_s *table, const char *name, struct object_type_s *type)
{
	int len;
	struct variable_s *var;

	if (!table)
		table = variable_root;
	for (len = 0;(name[len] != '\0') && (name[len] != NAME_SEPARATOR);len++) ;

	if (!(var = hash_find(table->hash, name, len)))
		return(NULL);
	else if (name[len] != '\0') {
		if (!VARIABLE_GET_TYPE(var)->index)
			return(NULL);
		return(VARIABLE_GET_TYPE(var)->index(var, &name[len + 1], type));
	}
	else {
		if (!type || (type == OBJECT_S(var)->type))
			return(var);
		return(NULL);
	}
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
int traverse_variable_table(struct variable_table_s *table, struct object_type_s *type, variable_traverse_func_t func, void *ptr)
{
	int ret = 0;
	struct variable_s *cur;

	if (!table)
		table = variable_root;

	hash_traverse_reset(table->hash);
        while ((cur = hash_traverse_next(table->hash))) {
		if ((OBJECT_S(cur)->type == type) && (ret = func(cur, ptr)))
			return(ret);
	}
	return(0);
}



