/*
 * Type Name:		attrib.c
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
	{ 0, 0, { SC_ENC_RGBA, 0x7F7F7F }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_RGBA, 0x00007F }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_RGBA, 0x007F00 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_RGBA, 0x7F0000 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_RGBA, 0x007F7F }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_RGBA, 0x7F7F00 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, 0, { SC_ENC_RGBA, 0x7F007F }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, SA_BOLD, { SC_ENC_RGBA, 0x7F7F7F }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, SA_BOLD, { SC_ENC_RGBA, 0x00007F }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, SA_BOLD, { SC_ENC_RGBA, 0x007F00 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, SA_BOLD, { SC_ENC_RGBA, 0x7F0000 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, SA_BOLD, { SC_ENC_RGBA, 0x007F7F }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, SA_BOLD, { SC_ENC_RGBA, 0x7F7F00 }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
	{ 0, SA_BOLD, { SC_ENC_RGBA, 0x7F007F }, { SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR } },
};

static struct type_s *attrib_type;

static void *fe_common_hashattrib_create(void *, char *, va_list);
static void fe_common_hashattrib_destroy(void *);
static void *fe_common_hashattrib_add(void *, struct type_s *, char *, int, char *, va_list);
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
		(type_add_t) fe_common_hashattrib_add,
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

static void *fe_common_hashattrib_add(void *value, struct type_s *type, char *name, int bitflags, char *params, va_list va)
{
	void *field;
	struct type_s *type_ptr;

	if (!(field = fe_common_hashattrib_index(value, name, &type_ptr)))
		return(NULL);
	if (type && (type != type_ptr))
		return(NULL);
	return(type_ptr->create(field, params, va));
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


