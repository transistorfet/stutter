/*
 * Header Name:		surface.h
 * Version:		0.1
 * Description:		Frontend Drawing Surface Header
 */

#ifndef _STUTTER_FRONTEND_SURFACE_H
#define _STUTTER_FRONTEND_SURFACE_H

#define SA_NORMAL		0x00
#define SA_BOLD			0x01
#define SA_INVERSE		0x02
#define SA_BLINK		0x04

#define SA_METHOD_SET		0x00
#define SA_METHOD_RESET		0x01
#define SA_METHOD_OR		0x02
#define SA_METHOD_TOGGLE	0x03

#define SC_ENC_RGBA		0x01
#define SC_ENC_MAPPING		0x02

#define SC_MAP_DEFAULT_COLOUR	0
#define SC_MAP_CURRENT_COLOUR	-1

#define SC_RGBA_BLACK		0x000000
#define SC_RGBA_RED		0xFF0000
#define SC_RGBA_GREEN		0x00FF00
#define SC_RGBA_YELLOW		0xFFFF00
#define SC_RGBA_BLUE		0x0000FF
#define SC_RGBA_MAGENTA		0xFF00FF
#define SC_RGBA_CYAN		0x00FFFF
#define SC_RGBA_WHITE		0xFFFFFF

#define SC_RGBA_ORANGE		0xFF7F00
#define SC_RGBA_PINK		0xFF007F
#define SC_RGBA_BROWN		0x7F7F00
#define SC_RGBA_GREY		0x7F7F7F
#define SC_RGBA_TURQUOISE	0x007F7F

#define SCC_GET_ATTRIB		0x0010
#define SCC_SET_ATTRIB		0x0020
#define SCC_MODIFY_ATTRIB	0x0030
#define SCC_RESIZE		0x0040

#define SUR_BF_UNDESTROYABLE	0x0001

#define surface_get_width_m(surface)		( ((struct surface_s *) surface)->width )
#define surface_get_height_m(surface)		( ((struct surface_s *) surface)->height )

typedef short style_t;

typedef struct colour_s {
	char enc;
	int colour;
} colour_t;

typedef struct attrib_s {
	char method;
	style_t style;
	colour_t fg;
	colour_t bg;
} attrib_t;

typedef surface_s;

#define surface_create_m(surface, parent, width, height, bitflags)	\
	((struct surface_s *) (surface))->type->create(((struct surface_s *) (surface)), parent, width, height, bitflags)
#define surface_destroy_m(surface)					\
	((struct surface_s *) (surface))->type->destroy(((struct surface_s *) (surface)))
#define surface_print_m(surface, str, len)				\
	((struct surface_s *) (surface))->type->print(((struct surface_s *) (surface)), str, len)
#define surface_clear_m(surface, x, y, width, height)			\
	((struct surface_s *) (surface))->type->clear(((struct surface_s *) (surface)), x, y, width, height)
#define surface_move_m(surface, x, y)			\
	((struct surface_s *) (surface))->type->move(((struct surface_s *) (surface)), x, y)
#define surface_control_m(surface, cmd, ...)				\
	((struct surface_s *) (surface))->type->control(((struct surface_s *) (surface)), cmd, __VA_ARGS__)

typedef struct surface_s *(*surface_create_t)(struct surface_s *parent, short width, short height, int bitflags);
typedef int (*surface_destroy_t)(struct surface_s *surface);
typedef int (*surface_print_t)(struct surface_s *surface, char *str, int len);
typedef void (*surface_clear_t)(struct surface_s *surface, short x, short y, short width, short height);
typedef void (*surface_move_t)(struct surface_s *surface, short x, short y);
typedef int (*surface_control_t)(struct surface_s *surface, int cmd, ...);

struct surface_type_s {
	char *name;
	int bitflags;
	surface_create_t create;
	surface_destroy_t destroy;
	surface_print_t print;
	surface_clear_t clear;
	surface_move_t move;
	surface_control_t control;
};

struct surface_s {
	struct surface_type_s *type;
	int bitflags;
	short x;
	short y;
	short width;
	short height;
};

/*** Miscellaneous Macros and Inline Functions ***/

static inline short surface_modify_style(char method, short org, short mod)
{
	switch (method) {
		case SA_METHOD_SET:
			return(mod);
		case SA_METHOD_RESET:
			return(org & ~mod);
		case SA_METHOD_OR:
			return(org | mod);
		case SA_METHOD_TOGGLE:
			return(org ^ mod);
		default:
			return(mod);
	}
}

#endif


