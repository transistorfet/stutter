/*
 * Header Name:		variable.h
 * Description:		Variable Manager Header
 */

#ifndef _STUTTER_VARIABLE_H
#define _STUTTER_VARIABLE_H

#include <stutter/hash.h>
#include <stutter/object.h>
#include <stutter/string.h>
#include <stutter/memory.h>

#define VAR_BF_NO_MODIFY	0x0001
#define VAR_BF_NO_REMOVE	0x0002

#define VT_ADD_REPLACE_OBJECT	0x1000

#define VAR_BF_SAVE_MASK	0x0FFF

#define VARIABLE_S(ptr)			( (struct variable_s *) (ptr) )
#define VARIABLE_TYPE_S(ptr)		( (struct variable_type_s *) (ptr) )
#define VARIABLE_GET_TYPE(ptr)		( VARIABLE_TYPE_S(OBJECT_S(ptr)->type) )

#define VARIABLE_TABLE_S(ptr)		( (struct variable_table_s *) (ptr) )
#define VARIABLE_TABLE_TYPE_S(ptr)	( (struct variable_table_type_s *) (ptr) )
#define VARIABLE_TABLE_GET_TYPE(ptr)	( VARIABLE_TABLE_TYPE_S(OBJECT_S(ptr)->type) )

struct variable_s;

typedef int (*variable_traverse_func_t)(struct variable_s *, void *);

typedef struct variable_s *(*variable_add_t)(struct variable_s *, struct object_type_s *, const char *, int, const char *, va_list);
typedef int (*variable_remove_t)(struct variable_s *, struct object_type_s *, const char *);
typedef struct variable_s *(*variable_index_t)(struct variable_s *, const char *, struct object_type_s *);
typedef int (*variable_traverse_t)(struct variable_s *, struct object_type_s *, variable_traverse_func_t, void *);
typedef int (*variable_stringify_t)(struct variable_s *, char *, int);
typedef int (*variable_evaluate_t)(struct variable_s *, char *);

struct variable_s {
	struct object_s object;
	char *name;
	int bitflags;
};

struct variable_table_s {
	struct variable_s variable;
	struct hash_s *hash;
};

struct variable_type_s {
	struct object_type_s object_type;
	variable_add_t add;
	variable_remove_t remove;
	variable_index_t index;
	variable_traverse_t traverse;
	variable_stringify_t stringify;
	variable_evaluate_t evaluate;
};

extern struct object_type_s variable_type;
extern struct variable_type_s variable_table_type;
extern struct variable_table_s *variable_root;

int init_variable(void);
int release_variable(void);

void variable_release(struct variable_s *);

int variable_table_init(struct variable_table_s *, const char *, va_list);
void variable_table_release(struct variable_table_s *);
struct variable_s *variable_table_add(struct variable_table_s *, struct object_type_s *, const char *, int, const char *, va_list);
int variable_table_remove(struct variable_table_s *, struct object_type_s *, const char *);
struct variable_s *variable_table_index(struct variable_table_s *, const char *, struct object_type_s *);
int variable_table_traverse(struct variable_table_s *, struct object_type_s *, variable_traverse_func_t, void *);

struct variable_s *add_variable(struct variable_s *, struct object_type_s *, const char *, int, const char *, ...);

/**
 * Returns true if the given char is a valid variable name char otherwise false.
 */
#define IS_VARIABLE_CHAR(ch)	\
	(((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')) || ((ch >= '0') && (ch <= '9')) || (ch == NAME_SEPARATOR) || (ch == '_') || (ch == '-'))

static inline int remove_variable(struct variable_s *var, struct object_type_s *type, const char *name)
{
	if (!var)
		var = VARIABLE_S(variable_root);
	if (!VARIABLE_GET_TYPE(var)->remove)
		return(-1);
	return(VARIABLE_GET_TYPE(var)->remove(var, type, name));
}

static inline struct variable_s *find_variable(struct variable_s *var, const char *name, struct object_type_s *type)
{
	if (!var)
		var = VARIABLE_S(variable_root);
	if (!VARIABLE_GET_TYPE(var)->index)
		return(NULL);
	return(VARIABLE_GET_TYPE(var)->index(var, name, type));
}

static inline int traverse_variable(struct variable_s *var, struct object_type_s *type, variable_traverse_func_t func, void *ptr)
{
	if (!var)
		var = VARIABLE_S(variable_root);
	if (!VARIABLE_GET_TYPE(var)->traverse)
		return(-1);
	return(VARIABLE_GET_TYPE(var)->traverse(var, type, func, ptr));
}

static inline int stringify_variable(struct variable_s *table, const char *name, char *buffer, int max)
{
	struct variable_s *var;

	if (!(var = find_variable(table, name, NULL)) || !VARIABLE_GET_TYPE(var)->stringify)
		return(-1);
	return(VARIABLE_GET_TYPE(var)->stringify(var, buffer, max));
}

static inline int evaluate_variable(struct variable_s *table, const char *name, char *args)
{
	struct variable_s *var;

	if (!(var = find_variable(table, name, NULL)) || !VARIABLE_GET_TYPE(var)->evaluate)
		return(-1);
	return(VARIABLE_GET_TYPE(var)->evaluate(var, args));
}

// TODO change the name of this function
static inline struct variable_s *index_variable(struct variable_s *table, const char *name, const char *index, struct object_type_s *type)
{
	struct variable_s *var;

	if (!(var = find_variable(table, name, NULL)) || !VARIABLE_GET_TYPE(var)->index)
		return(NULL);
	return(VARIABLE_GET_TYPE(var)->index(var, index, type));
}

#endif


