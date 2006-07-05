/*
 * Header Name:		variable.h
 * Version:		0.1
 * Description:		Variable Manager Header
 */

#ifndef _STUTTER_VARIABLE_H
#define _STUTTER_VARIABLE_H

#include <stutter/type.h>
#include <stutter/lib/string.h>

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

struct variable_s *add_variable(struct variable_table_s *, struct type_s *, char *, int, char *, ...);
struct variable_s *add_variable_real(struct variable_table_s *, struct type_s *, char *, int, char *, va_list);
int remove_variable(struct variable_table_s *, struct type_s *, char *);
struct variable_s *find_variable(struct variable_table_s *, char *);
int traverse_variable_table(struct variable_table_s *, struct type_s *, type_traverse_func_t, void *);

/**
 * Returns true if the given char is a valid variable name char otherwise false.
 */
#define is_variable_char_m(ch)	\
	(((str[i] >= 'A') && (str[i] <= 'Z')) || ((str[i] >= 'a') && (str[i] <= 'z')) || ((str[i] >= '0') && (str[i] <= '9')) || (str[i] == NAME_SEPARATOR) || (str[i] == '_') || (str[i] == '-'))

/**
 * Find the variable in the given table (or the root table if NULL) with the
 * given name and index it with the given index returning the variable found
 * or NULL on error.
 * 
 */
static inline struct variable_s *index_variable(struct variable_table_s *table, char *name, char *index)
{
	struct variable_s *variable;

	if (!(variable = find_variable(table, name)) || !variable->type->index)
		return(NULL);
	return(variable->type->index(variable->value, index));
}

/**
 * Find the variable in the given table (or the root table if NULL) with the
 * given name and stringify it storing the string in the given buffer up to
 * the maximum number of characters.  The number of characters written to the
 * buffer is returned or -1 on error.
 */
static inline int stringify_variable(struct variable_table_s *table, char *name, char *buffer, int max)
{
	struct variable_s *variable;

	if (!(variable = find_variable(table, name)) || !variable->type->stringify)
		return(-1);
	return(variable->type->stringify(variable->value, buffer, max));
}



#endif


