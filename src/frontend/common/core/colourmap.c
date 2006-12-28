/*
 * Module Name:		colourmap.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; memory
 * Description:		Colour Map Manager
 */

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/lib/memory.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/common/colourmap.h>

int colourmap_defaults[FE_COLOURMAP_SIZE] = {
	0x7F7F7F,
	0x000000,
	0x00007F,
	0x007F00,
	0x7F0000,
	0x007F7F,
	0x7F007F,
	0x7F7F00,
	0xFFFF00,
	0x00FF00,
	0x00FFFF,
	0xFF0000,
	0x0000FF,
	0xFF00FF,
	0x7F7F7F,
	0xFFFFFF
};

struct colourmap_s *colourmap_list = NULL;

int init_colourmap(void)
{
	if (colourmap_list)
		return(1);
	if (!(colourmap_list = create_colourmap(FE_COLOURMAP_SIZE)))
		return(-1);
	colourmap_load_defaults(colourmap_list);
	return(0);
}

int release_colourmap(void)
{
	if (!colourmap_list)
		return(1);
	destroy_colourmap(colourmap_list);
	return(0);
}

struct colourmap_s *create_colourmap(int size)
{
	struct colourmap_s *map;

	if (!(map = (struct colourmap_s *) memory_alloc(sizeof(struct colourmap_s) + (sizeof(colour_t) * size))))
		return(NULL);
	map->size = size;
	map->map = (colour_t *) (((size_t) map) + sizeof(struct colourmap_s));
	return(map);
}

int destroy_colourmap(struct colourmap_s *map)
{
	memory_free(map);
	return(0);
}

int colourmap_set_colour(struct colourmap_s *map, char enc, int index, int colour)
{
	if (!map)
		map = colourmap_list;
	if (index >= map->size)
		return(-1);

	switch (enc) {
		case SC_ENC_RGBA: {
			map->map[index].enc = SC_ENC_RGBA;
			map->map[index].colour = colour;
			break;
		}
		case SC_ENC_MAPPING: {
			if ((colour == index) || (map->map[index].enc == SC_ENC_MAPPING))
				return(-1);
			map->map[index].enc = SC_ENC_MAPPING;
			map->map[index].colour = colour;
			break;
		}
		default:
			return(-1);
	}
	return(0);
}

int colourmap_get_colour(struct colourmap_s *map, char enc, int index)
{
	if (!map)
		map = colourmap_list;
	if (index >= map->size)
		return(-1);

	switch (enc) {
		case SC_ENC_MAPPING: {
			if (map->map[index].enc == SC_ENC_MAPPING)
				return(map->map[index].colour);
			return(index);
		}
		case SC_ENC_RGBA: {
			switch (map->map[index].enc) {
				case SC_ENC_RGBA:
					return(map->map[index].colour);
				case SC_ENC_MAPPING:
					if (map->map[map->map[index].colour].enc != SC_ENC_RGBA)
						return(0);
					return(map->map[map->map[index].colour].enc);
				default:
					return(0);
			}
		}
		default:
			return(0);
	}
}

void colourmap_load_defaults(struct colourmap_s *map)
{
	int i;

	for (i = 0;(i < map->size) && (i < FE_COLOURMAP_SIZE);i++) {
		map->map[i].enc = SC_ENC_RGBA;
		map->map[i].colour = colourmap_defaults[i];
	}

	for (;i < map->size;i++) {
		map->map[i].enc = SC_ENC_RGBA;
		map->map[i].colour = 0x00000000;
	}
}

void colourmap_load_zeros(struct colourmap_s *map)
{
	int i;

	for (i = 0;i < map->size;i++) {
		map->map[i].enc = SC_ENC_RGBA;
		map->map[i].colour = 0x00000000;
	}
}
