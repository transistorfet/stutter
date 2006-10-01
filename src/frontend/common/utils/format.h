/*
 * Header Name:		format.h
 * Version:		0.1
 * Description:		Format Parser Header
 */

#ifndef _FRONTEND_COMMON_FORMAT_H
#define _FRONTEND_COMMON_FORMAT_H

#include <stutter/frontend/surface.h>

#define FORMAT_MAX_STYLES		64

#define format_string_size_m(format)	( sizeof(struct format_string_s) + ( (format)->length + 1 ) \
					 + ( (format)->num_styles * sizeof(struct format_style_s) ) )

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

struct format_string_s *create_format_string(const char *, int);
int destroy_format_string(struct format_string_s *);
int copy_format_string(struct format_string_s *, struct format_string_s *, int, int);

#endif


