/*
 * Header Name:		variable.h
 * Version:		0.1
 * Description:		Variable Manager Header
 */

#ifndef _VARIABLE_H
#define _VARIABLE_H

#include <nit/string.h>

#include <type.h>

int init_variable(void);
int release_variable(void);

int add_variable(struct type_s *, char *, char *, void *);
int remove_variable(char *, char *);
void *find_variable(struct type_s *, char *, char *);

#endif


