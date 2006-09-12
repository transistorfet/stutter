/*
 * Header Name:		format.h
 * Version:		0.1
 * Description:		Format Parser Header
 */

#ifndef _FRONTEND_COMMON_FORMAT_H
#define _FRONTEND_COMMON_FORMAT_H

#include <stutter/frontend/surface.h>

#define FORMAT_MAX_STYLES		64

struct format_style_s {
	int index;
	attrib_t attrib;
};

struct format_string_s {
	char *str;
	int length;
	int num_styles;
	struct format_style_s *styles;
};

struct format_string_s *create_format_string(const char *);
int destroy_format_string(struct format_string_s *);

#endif


