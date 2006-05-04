/*
 * Type Name:		table.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; memory
 * Description:		Command Type
 */

#include <string.h>
#include <stdarg.h>

#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/lib/memory.h>


static void *base_table_create(void *, char *, va_list);

struct type_s *base_load_table(void)
{
	return(add_type(
		"table",
		0,
		(type_create_t) base_table_create,
		(type_destroy_t) destroy_variable_table,
		(type_add_t) add_variable_real,
		(type_remove_t) remove_variable,
		(type_index_t) find_variable,
		(type_traverse_t) traverse_variable_table,
		NULL,
		NULL
	));
}

static void *base_table_create(void *value, char *str, va_list va)
{
	if (*str != '\0')
		return(NULL);
	else if (value)
		return(value);
	else
		return(create_variable_table());
}


