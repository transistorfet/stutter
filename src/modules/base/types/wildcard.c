/*
 * Type Name:		wildcard.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; memory
 * Description:		Wildcard Type
 */

#include <string.h>
#include <stdarg.h>

#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/lib/memory.h>


static struct variable_s *base_wildcard_index(char *, char *);

struct type_s *base_load_wildcard(void)
{
	return(add_type(
		"wildcard",
		0,
		(type_create_t) recreate_string_real,
		(type_destroy_t) destroy_string,
		NULL,
		NULL,
		(type_index_t) base_wildcard_index,
		NULL,
		(type_stringify_t) copy_string,
		NULL
	));
}

static struct variable_s *base_wildcard_index(char *value, char *name)
{
	int i = 0;
	char ch, *str;
	struct variable_s *table, *variable;

	if (!value)
		return(NULL);
	str = value;
	do {
		if ((value[i] == ';') || (value[i] == '\0')) {
			ch = value[i];
			value[i] = '\0';
			if (((*str == '\0') && (variable = find_variable(NULL, name)))
			    || ((table = find_variable(NULL, str)) && table->type->index && (variable = table->type->index(table->value, name)))) {
				value[i] = ch;
				return(variable);
			}
			if (ch != '\0') {
				value[i] = ch;
				str = &value[i + 1];
			}
		}
	} while (value[i++] != '\0');
	return(NULL);
}

