/*
 * Header Name:		variable.h
 * Version:		0.1
 * Description:		Variable Manager Header
 */

#ifndef _STUTTER_VARIABLE_H
#define _STUTTER_VARIABLE_H

#include <stutter/type.h>
#include <stutter/string.h>
#include <stutter/memory.h>

#define VAR_ERR_NOT_FOUND	-10

#define VAR_BF_NO_MODIFY	0x0001
#define VAR_BF_NO_REMOVE	0x0002

struct variable_s {
	struct type_s *type;
	char *name;
	int bitflags;
	void *value;
};

struct variable_table_s;

int init_variable(void);
int release_variable(void);

struct variable_table_s *create_variable_table(void);
int destroy_variable_table(struct variable_table_s *);

void *add_variable(struct variable_table_s *, struct type_s *, char *, int, char *, ...);
void *add_variable_real(struct variable_table_s *, struct type_s *, char *, int, char *, va_list);
int remove_variable(struct variable_table_s *, struct type_s *, char *);
void *find_variable(struct variable_table_s *, char *, struct type_s **);
int traverse_variable_table(struct variable_table_s *, struct type_s *, type_traverse_func_t, void *);

/**
 * Returns true if the given char is a valid variable name char otherwise false.
 */
#define is_variable_char_m(ch)	\
	(((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')) || ((ch >= '0') && (ch <= '9')) || (ch == NAME_SEPARATOR) || (ch == '_') || (ch == '-'))

/**
 * Find the variable in the given table (or the root table if NULL) with the
 * given name and index it with the given index returning the variable found
 * or NULL on error.
 * 
 */
static inline void *index_variable(struct variable_table_s *table, char *name, char *index, struct type_s **type_ptr)
{
	void *value;
	struct type_s *type;

	if (!(value = find_variable(table, name, &type)) || !type || !type->index)
		return(NULL);
	return(type->index(value, index, type_ptr));
}

/**
 * Find the variable in the given table (or the root table if NULL) with the
 * given name and stringify it storing the string in the given buffer up to
 * the maximum number of characters.  The number of characters written to the
 * buffer is returned or -1 on error.
 */
static inline int stringify_variable(struct variable_table_s *table, char *name, char *buffer, int max)
{
	void *value;
	struct type_s *type;

	if (!(value = find_variable(table, name, &type)) || !type->stringify)
		return(VAR_ERR_NOT_FOUND);
	return(type->stringify(value, buffer, max));
}

/**
 * Find the variable in the given table (or the root table if NULL) with the
 * given name and evaluate it using the given args string.  The value
 * returned by the evaluation is returned.
 */
static inline int evaluate_variable(struct variable_table_s *table, char *name, char *args)
{
	void *value;
	struct type_s *type;

	if (!(value = find_variable(table, name, &type)) || !type->stringify)
		return(VAR_ERR_NOT_FOUND);
	return(type->evaluate(value, args));
}

#endif


