/*
 * Header Name:		colourmap.h
 * Description:		Colour Map Manager Interface
 */

#ifndef _STUTTER_FRONTEND_COMMON_COLOURMAP_H
#define _STUTTER_FRONTEND_COMMON_COLOURMAP_H

struct fe_colourmap {
	int size;
	colour_t *map;
};

int init_colourmap(void);
int release_colourmap(void);
struct fe_colourmap *create_colourmap(int size);
int destroy_colourmap(struct fe_colourmap *map);
int colourmap_set_colour(struct fe_colourmap *map, char enc, int index, int colour);
int colourmap_get_colour(struct fe_colourmap *map, char enc, int index);
void colourmap_load_defaults(struct fe_colourmap *map);
void colourmap_load_zeros(struct fe_colourmap *map);

#endif

