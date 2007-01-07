/*
 * Type Name:		style.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; string ; modbase
 * Description:		Style Type
 */

#include <stdlib.h>

#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/string.h>
#include <stutter/frontend/surface.h>

static void *fe_common_style_create(style_t *, char *, va_list);
static int fe_common_style_stringify(style_t *, char *, int);

struct type_s *fe_common_load_style(void)
{
	return(add_type(
		"style:fe",
		0,
		(type_create_t) fe_common_style_create,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(type_stringify_t) fe_common_style_stringify,
		NULL
	));
}

/*** Local Functions ***/

static void *fe_common_style_create(style_t *value, char *params, va_list va)
{
	char *str;

	if (!strcmp(params, "pointer"))
		return(va_arg(va, void *));
	else if (!value)
		return(NULL);
	else if (!strcmp(params, "string")) {
		str = va_arg(va, char *);
		*value = 0;
		if (strstr(str, "bold"))
			*value |= SA_BOLD;
		if (strstr(str, "inverse"))
			*value |= SA_INVERSE;
	}
	return(value);
}

static int fe_common_style_stringify(style_t *value, char *buffer, int max)
{
	int i = 0;

	if (*value == SA_NORMAL) {
		if ((i += snprintf(&buffer[i], max - i, "normal ")) < 0)
			return(-1);
	}
	else {
		if ((*value & SA_BOLD) && ((i += snprintf(&buffer[i], max - i, "bold ")) < 0))
			return(-1);
		if ((*value & SA_INVERSE) && ((i += snprintf(&buffer[i], max - i, "inverse ")) < 0))
			return(-1);
	}
	return(i);
}


