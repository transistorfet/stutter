/*
 * Type Name:		attrib.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; string ; modbase
 * Type Requirements:	colour:fe ; style:fe
 * Description:		Attribute Type
 */

#include <stdlib.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>
#include <stutter/lib/memory.h>
#include <stutter/frontend/surface.h>

static struct type_s *style_type;
static struct type_s *colour_type;

static void *fe_common_attrib_create(attrib_t *, char *, va_list);
static void fe_common_attrib_destroy(attrib_t *);
static void *fe_common_attrib_add(attrib_t *, struct type_s *, char *, int, char *, va_list);
static void *fe_common_attrib_index(attrib_t *, char *, struct type_s **);
static int fe_common_attrib_stringify(attrib_t *, char *, int);

struct type_s *fe_common_load_attrib(void)
{
	if (!(style_type = find_type("style:fe")))
		return(NULL);
	if (!(colour_type = find_type("colour:fe")))
		return(NULL);

	return(add_type(
		"attrib:fe",
		0,
		(type_create_t) fe_common_attrib_create,
		(type_destroy_t) fe_common_attrib_destroy,
		(type_add_t) fe_common_attrib_add,
		NULL,
		(type_index_t) fe_common_attrib_index,
		NULL,
		(type_stringify_t) fe_common_attrib_stringify,
		NULL
	));
}

/*** Local Functions ***/

static void *fe_common_attrib_create(attrib_t *value, char *params, va_list va)
{
	int i;
	char *str;
	char buffer[STRING_SIZE];

	if (!value && !(value = memory_alloc(sizeof(attrib_t))))
		return(NULL);

	if (!strcmp(params, "string")) {
		str = va_arg(va, char *);
		if (!strcmp_icase(str, "default")) {
			value->style = SA_NORMAL;
			value->fg.enc = SC_ENC_MAPPING;
			value->fg.colour = SC_MAP_DEFAULT_COLOUR;
			value->bg.enc = SC_ENC_MAPPING;
			value->bg.colour = SC_MAP_DEFAULT_COLOUR;
		}
		else {
			i = strcpy_lcase(str, buffer, STRING_SIZE, ',');
			type_create(colour_type, &value->fg, "string", buffer);
			i += strcpy_lcase(&str[i], buffer, STRING_SIZE, ',');
			type_create(colour_type, &value->bg, "string", buffer);
			i += strcpy_lcase(&str[i], buffer, STRING_SIZE, ',');
			type_create(style_type, &value->style, "string", buffer);
		}
	}
	return(value);
}

static void fe_common_attrib_destroy(attrib_t *value)
{
	if (value)
		memory_free(value);
}

static void *fe_common_attrib_add(attrib_t *value, struct type_s *type, char *name, int bitflags, char *params, va_list va)
{
	void *field;
	struct type_s *type_ptr;

	if (!(field = fe_common_attrib_index(value, name, &type_ptr)))
		return(NULL);
	if (type && (type != type_ptr))
		return(NULL);
	return(type_ptr->create(field, params, va));
}

static void *fe_common_attrib_index(attrib_t *value, char *name, struct type_s **type_ptr)
{
	if (!strcmp(name, "fg")) {
		if (type_ptr)
			*type_ptr = colour_type;
		return(&value->fg);
	}
	else if (!strcmp(name, "bg")) {
		if (type_ptr)
			*type_ptr = colour_type;
		return(&value->bg);
	}
	else if (!strcmp(name, "style")) {
		if (type_ptr)
			*type_ptr = style_type;
		return(&value->style);
	}
	else
		return(NULL);
}

static int fe_common_attrib_stringify(attrib_t *value, char *buffer, int max)
{
	int i = 0;
	int no_bg = 1;

	if ((value->fg.enc != SC_ENC_MAPPING) || (value->fg.colour != SC_MAP_CURRENT_COLOUR))
		i += colour_type->stringify(&value->fg, buffer, max);
	if ((value->bg.enc != SC_ENC_MAPPING) || (value->bg.colour != SC_MAP_CURRENT_COLOUR)) {
		no_bg = 0;
		buffer[i++] =',';
		i += colour_type->stringify(&value->bg, &buffer[i], max - i);
	}
	if (value->style != SA_NORMAL) {
		if (no_bg)
			buffer[i++] =',';
		buffer[i++] =',';
		i += style_type->stringify(&value->style, &buffer[i], max - i);
	}
	buffer[i] = '\0';
	return(i);
}


