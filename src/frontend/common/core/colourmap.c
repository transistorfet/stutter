/*
 * Module Name:		colourmap.c
 * Description:		Colour Map Manager
 */

#include <stdarg.h>

#include CONFIG_H
#include <stutter/object.h>
#include <stutter/memory.h>
#include <stutter/macros.h>
#include <stutter/variable.h>
#include <stutter/frontend/common/surface.h>
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

struct variable_type_s fe_colourmap_type = { {
	OBJECT_TYPE_S(&variable_type),
	"fe_colourmap",
	sizeof(struct fe_colourmap),
	NULL,
	(object_init_t) fe_colourmap_init,
	(object_release_t) fe_colourmap_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) NULL,
	(variable_traverse_t) NULL,
	(variable_stringify_t) NULL,
	(variable_evaluate_t) NULL
};

struct fe_colourmap *colourmap_list = NULL;

int init_colourmap(void)
{
	if (colourmap_list)
		return(0);
	if (!(colourmap_list = FE_COLOURMAP(create_object(OBJECT_TYPE_S(&fe_colourmap_type), "n", FE_COLOURMAP_SIZE))))
		return(-1);
	fe_colourmap_load_defaults(colourmap_list);
	return(0);
}

int release_colourmap(void)
{
	if (!colourmap_list)
		return(0);
	destroy_object(OBJECT_S(colourmap_list));
	return(0);
}

int fe_colourmap_init(struct fe_colourmap *map, const char *params, va_list va)
{
	int size = FE_COLOURMAP_SIZE;

	if (params[0] == 'n')
		size = va_arg(va, int);
	if (!(map->map = (colour_t *) memory_alloc(sizeof(colour_t) * size)))
		return(-1);
	map->size = size;
	return(0);
}

void fe_colourmap_release(struct fe_colourmap *map)
{
	memory_free(map->map);
}

int fe_colourmap_set_colour(struct fe_colourmap *map, char enc, int index, int colour)
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

int fe_colourmap_get_colour(struct fe_colourmap *map, char enc, int index)
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

void fe_colourmap_load_defaults(struct fe_colourmap *map)
{
	int i;

	for (i = 0; (i < map->size) && (i < FE_COLOURMAP_SIZE); i++) {
		map->map[i].enc = SC_ENC_RGBA;
		map->map[i].colour = colourmap_defaults[i];
	}

	for (; i < map->size; i++) {
		map->map[i].enc = SC_ENC_RGBA;
		map->map[i].colour = 0x00000000;
	}
}

void fe_colourmap_load_zeros(struct fe_colourmap *map)
{
	int i;

	for (i = 0; i < map->size; i++) {
		map->map[i].enc = SC_ENC_RGBA;
		map->map[i].colour = 0x00000000;
	}
}

