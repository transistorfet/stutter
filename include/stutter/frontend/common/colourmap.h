/*
 * Header Name:		colourmap.h
 * Description:		Colour Map Manager Interface
 */

#ifndef _STUTTER_FRONTEND_COMMON_COLOURMAP_H
#define _STUTTER_FRONTEND_COMMON_COLOURMAP_H

#include <stdarg.h>

#include <stutter/object.h>
#include <stutter/variable.h>

#define FE_COLOURMAP(ptr)			( (struct fe_colourmap *) (ptr) )

struct fe_colourmap {
	int size;
	colour_t *map;
};

extern struct variable_type_s fe_colourmap_type;

int init_colourmap(void);
int release_colourmap(void);

int fe_colourmap_init(struct fe_colourmap *, const char *, va_list);
void fe_colourmap_release(struct fe_colourmap *);
int fe_colourmap_set_colour(struct fe_colourmap *, char, int, int);
int fe_colourmap_get_colour(struct fe_colourmap *, char, int);
void fe_colourmap_load_defaults(struct fe_colourmap *);
void fe_colourmap_load_zeros(struct fe_colourmap *);

#endif

