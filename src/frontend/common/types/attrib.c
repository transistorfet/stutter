/*
 * Type Name:		attrib.c
 * Type Requirements:	colour:fe ; style:fe
 * Description:		Attribute Type
 */

#include <stdlib.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/memory.h>
#include <stutter/variable.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/common/types.h>

struct variable_type_s fe_attrib_type = { {
	OBJECT_TYPE_S(&variable_type),
	"attrib",
	sizeof(struct fe_attrib),
	NULL,
	(object_init_t) fe_attrib_init,
	(object_release_t) fe_attrib_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) NULL,
	(variable_evaluate_t) base_command_evaluate
};


int fe_attrib_init(struct fe_attrib *var, const char *params, va_list va)
{
	int i;
	char *str;
	char buffer[SMALL_STRING_SIZE];

	if (params[0] != 's')
		return(-1);
	str = va_arg(va, char *);
	if (!strcmp_icase(str, "default")) {
		var->attrib.style = SA_NORMAL;
		var->attrib.fg.enc = SC_ENC_MAPPING;
		var->attrib.fg.colour = SC_MAP_DEFAULT_COLOUR;
		var->attrib.bg.enc = SC_ENC_MAPPING;
		var->attrib.bg.colour = SC_MAP_DEFAULT_COLOUR;
	}
	else {
		i = strcpy_lcase(str, buffer, SMALL_STRING_SIZE, ',');
		type_create(colour_type, &value->fg, "string", buffer);
		i += strcpy_lcase(&str[i], buffer, SMALL_STRING_SIZE, ',');
		type_create(colour_type, &value->bg, "string", buffer);
		i += strcpy_lcase(&str[i], buffer, SMALL_STRING_SIZE, ',');
		type_create(style_type, &value->style, "string", buffer);
	}
	return(0);
}

int fe_attrib_stringify(struct fe_attrib *var, char *buffer, int max)
{
	int i = 0;
	int no_bg = 1;

	if ((var->attrib.fg.enc != SC_ENC_MAPPING) || (var->attrib.fg.colour != SC_MAP_CURRENT_COLOUR))
		i += colour_type->stringify(&value->fg, buffer, max);
	if ((var->attrib.bg.enc != SC_ENC_MAPPING) || (var->attrib.colour != SC_MAP_CURRENT_COLOUR)) {
		no_bg = 0;
		buffer[i++] =',';
		i += colour_type->stringify(&var->attrib.bg, &buffer[i], max - i);
	}
	if (var->attrib.style != SA_NORMAL) {
		if (no_bg)
			buffer[i++] =',';
		buffer[i++] =',';
		i += style_type->stringify(&var->attrib.style, &buffer[i], max - i);
	}
	buffer[i] = '\0';
	return(i);
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


