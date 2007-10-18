/*
 * Type Name:		string.c
 * Description:		String Type
 */

#include <stdlib.h>

#include <stutter/type.h>
#include <stutter/string.h>
#include <stutter/modules/base.h>

struct type_s *base_load_string(void)
{
	return(add_type(
		"string",
		0,
		(type_create_t) type_recreate_string,
		(type_destroy_t) destroy_string,
		NULL,
		NULL,
		NULL,
		NULL,
		(type_stringify_t) copy_string,
		NULL
	));
}


