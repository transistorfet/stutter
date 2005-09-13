/*
 * Header Name:		variable.h
 * Version:		0.1
 * Description:		Variable Manager Header
 */

#ifndef _VARIABLE_H
#define _VARIABLE_H

#include <nit/string.h>

int init_variable(void);
int release_variable(void);

int add_variable(char *, string_t);
int remove_variable(char *);
string_t find_variable(char *);

#endif


