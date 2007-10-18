/*
 * Type Name:		wildcard.c
 * Description:		Wildcard Type
 */

#include <string.h>
#include <stdarg.h>

#include <stutter/type.h>
#include <stutter/memory.h>
#include <stutter/variable.h>

static void *base_wildcard_index(char *, char *, struct type_s **);

struct type_s *base_load_wildcard(void)
{
	return(add_type(
		"wildcard",
		0,
		(type_create_t) type_recreate_string,
		(type_destroy_t) destroy_string,
		NULL,
		NULL,
		(type_index_t) base_wildcard_index,
		NULL,
		(type_stringify_t) copy_string,
		NULL
	));
}

static void *base_wildcard_index(char *wildcard, char *name, struct type_s **type_ptr)
{
	int i = 0;
	void *value;
	char ch, *str;

	if (!wildcard)
		return(NULL);
	str = wildcard;
	do {
		if ((wildcard[i] == ';') || (wildcard[i] == '\0')) {
			ch = wildcard[i];
			wildcard[i] = '\0';
			if (((*str == '\0') && (value = find_variable(NULL, name, type_ptr))) || (value = index_variable(NULL, str, name, type_ptr))) {
				wildcard[i] = ch;
				return(value);
			}
			if (ch != '\0') {
				wildcard[i] = ch;
				str = &wildcard[i + 1];
			}
		}
	} while (wildcard[i++] != '\0');
	return(NULL);
}

