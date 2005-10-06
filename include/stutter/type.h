/*
 * Header Name:		type.h
 * Version:		0.1
 * Description:		Type Manager Header
 */

#ifndef _TYPE_H
#define _TYPE_H

#include <nit/types.h>
#include <nit/string.h>
#include <nit/callback.h>

struct type_s {
	char *name;
	destroy_t destroy;
};

int init_type(void);
int release_type(void);

int add_type(char *, destroy_t);
int remove_type(char *);
struct type_s *find_type(char *);

#endif


