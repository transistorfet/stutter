/*
 * Type Name:		attrib.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; string ; modbase
 * Type Requirements:	attrib:fe
 * Description:		Attribute Type
 */

#include <stdlib.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/memory.h>
#include <stutter/frontend/surface.h>

static attrib_t attrib_table[] = {
	{ 0, 0, { SC_ENC_MAPPING, 1 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 2 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 3 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 4 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 5 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 6 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 7 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 8 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 9 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 10 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 11 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 12 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 13 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 14 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_MAPPING, 15 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } }
};

static struct type_s *attrib_type;

static void *fe_common_hashattrib_create(void *, char *, va_list);
static void fe_common_hashattrib_destroy(void *);
static void *fe_common_hashattrib_index(void *, char *, struct type_s **);

struct type_s *fe_common_load_hashattrib(void)
{
	if (!(attrib_type = find_type("attrib:fe")))
		return(NULL);

	return(add_type(
		"hashattrib:fe",
		0,
		(type_create_t) fe_common_hashattrib_create,
		(type_destroy_t) fe_common_hashattrib_destroy,
		NULL,
		NULL,
		(type_index_t) fe_common_hashattrib_index,
		NULL,
		NULL,
		NULL
	));
}

/*** Local Functions ***/

static void *fe_common_hashattrib_create(void *value, char *params, va_list va)
{
	if (*params != '\0')
		return(NULL);
	return((void *) 1);
}

static void fe_common_hashattrib_destroy(void *value)
{
	return;
}

static void *fe_common_hashattrib_index(void *value, char *name, struct type_s **type_ptr)
{
	int i, hash;

	for (i = 0, hash = 0;name[i] != '\0';i++)
		hash += name[i];

	if (type_ptr)
		*type_ptr = attrib_type;
	return(&(attrib_table[hash % (sizeof(attrib_table) / sizeof(*attrib_table))]));
}


