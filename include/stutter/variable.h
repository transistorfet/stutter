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

#endif


