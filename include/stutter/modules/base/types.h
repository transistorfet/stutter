/*
 * Header Name:		types.h
 * Description:		Base Module Types Header
 */

#ifndef _STUTTER_MODULES_BASE_TYPES_H
#define _STUTTER_MODULES_BASE_TYPES_H

#include <stdlib.h>

#include <stutter/object.h>
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/variable.h>

/*** Command Type ***/

#define BASE_COMMAND_S(ptr)			( (struct base_command_s *) (ptr) )

struct base_command {
	struct variable_s variable;
	callback_t func;
	void *ptr;
};

extern struct variable_type_s base_command_type;

int base_command_init(struct base_command *, const char *, va_list);
void base_command_release(struct base_command *);
int base_command_evaluate(struct base_command *, void *);

/*** String Type ***/

#define BASE_STRING_S(ptr)		( (struct base_string_s *) (ptr) )

struct base_string {
	struct variable_s variable;
	string_t str;
};

extern struct variable_type_s base_string_type;

int base_string_init(struct base_string *, const char *, va_list);
void base_string_release(struct base_string *);
int base_string_stringify(struct base_string *, char *, int);

/*** Format Type ***/

extern struct variable_type_s base_format_type;

int base_format_stringify(struct base_string *, char *, int);

/*** Status Type ***/

struct base_status {
	stringify_t func;
	void *ptr;
};

extern struct variable_type_s base_status_type;

int base_status_init(struct base_status *, const char *, va_list);
void base_status_release(struct base_status *);
int base_status_stringify(struct base_status *, char *, int);

/*** Time Type ***/

extern struct variable_type_s base_time_type;

int base_time_stringify(struct base_string *, char *, int);

/*** Wildcard Type ***/

extern struct variable_type_s base_wildcard_type;

struct variable_s *base_wildcard_index(struct base_string *, const char *, struct object_type_s *);




struct type_s *base_load_string_ref(void);
struct type_s *base_load_timer(void);

#endif


