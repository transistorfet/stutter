/*
 * Header Name:		type.h
 * Version:		0.1
 * Description:		Type Manager Header
 */

#ifndef _TYPE_H
#define _TYPE_H

#include <nit/types.h>

typedef int (*create_t)(char *, ...);
typedef int (*evaluate_t)(void *, void *);

struct type_s {
	char *name;
	create_t create;
	evaluate_t evaluate;
	destroy_t destroy;
};

int init_type(void);
int release_type(void);

struct type_s *add_type(char *, create_t, evaluate_t, destroy_t);
int remove_type(char *);
struct type_s *find_type(char *);

#endif


