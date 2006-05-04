/*
 * Header Name:		type.h
 * Version:		0.1
 * Description:		Type Manager Header
 */

#ifndef _STUTTER_TYPE_H
#define _STUTTER_TYPE_H

#include <stdarg.h>
#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>

struct type_s;
struct variable_s;

typedef int (*type_traverse_func_t)(struct variable_s *, void *);

typedef void *(*type_create_t)(void *, char *, va_list);
typedef void (*type_destroy_t)(void *);
typedef struct variable_s *(*type_add_t)(void *, struct type_s *, char *, int, char *, va_list);
typedef int (*type_remove_t)(void *, struct type_s *, char *);
typedef struct variable_s *(*type_index_t)(void *, char *);
typedef int (*type_traverse_t)(void *, type_traverse_func_t, struct type_s *, void *);
// TODO should stringify take a buffer and fill it instead of returning a string_t?
typedef string_t (*type_stringify_t)(void *);
typedef int (*type_evaluate_t)(void *, char *);

struct type_s {
	char *name;
	int bitflags;
	type_create_t create;
	type_destroy_t destroy;
	type_add_t add;
	type_remove_t remove;
	type_index_t index;
	type_traverse_t traverse;
	type_stringify_t stringify;
	type_evaluate_t evaluate;
};

int init_type(void);
int release_type(void);

struct type_s *add_type(char *, int, type_create_t, type_destroy_t, type_add_t, type_remove_t, type_index_t, type_traverse_t, type_stringify_t, type_evaluate_t);
int remove_type(char *);
struct type_s *find_type(char *);

#endif


