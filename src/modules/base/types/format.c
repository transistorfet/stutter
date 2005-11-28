/*
 * Type Name:		format.c
 * Version:		0.1
 * Module Requirements:	type ; string ; modbase
 * Description:		Format Type
 */

#include <stdlib.h>

#include <type.h>
#include <nit/string.h>
#include <modules/base.h>

struct type_s *base_load_format(void)
{
	return(add_type("format", (create_t) create_string, (stringify_t) base_expand_str, NULL, (destroy_t) destroy_string));
}


