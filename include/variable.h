/*
 * Header Name:		variable.h
 * Version:		0.1
 * Description:		Variable Manager Header
 */

#ifndef _VARIABLE_H
#define _VARIABLE_H

#include <type.h>
#include <namespace.h>
#include <lib/string.h>

struct variable_s {
	struct type_s *type;
	struct namespace_s *ns;
	char *name;
	void *value;
};

int init_variable(void);
int release_variable(void);

struct variable_s *add_variable(struct type_s *, char *, char *, void *);
int remove_variable(struct type_s *, char *, char *);
struct variable_s *find_variable(struct type_s *, char *, char *);
int select_variable_path(char *);

#endif


