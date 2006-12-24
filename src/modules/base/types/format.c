/*
 * Type Name:		format.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; string ; modbase
 * Description:		Format String Type
 */

#include <stdlib.h>

#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/lib/string.h>
#include <stutter/modules/base.h>

struct type_s *base_load_format(void)
{
	return(add_type(
		"format",
		0,
		(type_create_t) type_recreate_string,
		(type_destroy_t) destroy_string,
		NULL,
		NULL,
		NULL,
		NULL,
		(type_stringify_t) util_expand_str,
		NULL
	));
}


