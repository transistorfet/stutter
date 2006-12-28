/*
 * Header Name:		colourmap.h
 * Version:		0.1
 * Description:		Colour Map Manager Interface
 */

#ifndef _STUTTER_FRONTEND_COMMON_COLOURMAP_H
#define _STUTTER_FRONTEND_COMMON_COLOURMAP_H

struct colourmap_s {
	int size;
	colour_t *map;
};

int init_colourmap(void);
int release_colourmap(void);
struct colourmap_s *create_colourmap(int size);
int destroy_colourmap(struct colourmap_s *map);
int colourmap_set_colour(struct colourmap_s *map, char enc, int index, int colour);
int colourmap_get_colour(struct colourmap_s *map, char enc, int index);
void colourmap_load_defaults(struct colourmap_s *map);
void colourmap_load_zeros(struct colourmap_s *map);

#endif

