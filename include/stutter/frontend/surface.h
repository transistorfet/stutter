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

#define SA_METHOD		0xf0000000
#define SA_SET			0x00000000
#define SA_RESET		0x10000000
#define SA_OR			0x20000000
#define SA_TOGGLE		0x30000000

#define SA_BLACK		0x000000
#define SA_RED			0xFF0000
#define SA_GREEN		0x00FF00
#define SA_YELLOW		0xFFFF00
#define SA_BLUE			0x0000FF
#define SA_MAGENTA		0xFF00FF
#define SA_CYAN			0x00FFFF
#define SA_WHITE		0xFFFFFF

#define SA_ORANGE		0xFF7F00
#define SA_PINK			0xFF007F
#define SA_BROWN		0x7F7F00
#define SA_GREY			0x7F7F7F
#define SA_TURQUOISE		0x007F7F

#define SCC_GET_ATTRIB		0x0010
#define SCC_SET_ATTRIB		0x0020
#define SCC_MODIFY_ATTRIB	0x0030
#define SCC_RESIZE		0x0040

#define SUR_BF_UNDESTROYABLE	0x0001

#define surface_get_width_m(surface)		( ((struct surface_s *) surface)->width )
#define surface_get_height_m(surface)		( ((struct surface_s *) surface)->height )

typedef struct attrib_s {
	int fg;
	int bg;
	int attrib;
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

static inline int surface_modify_attrib(int org, int mod)
{
	switch (mod & SA_METHOD) {
		case SA_SET:
			return(mod & ~SA_METHOD);
		case SA_RESET:
			return(org & ~(mod & ~SA_METHOD));
		case SA_OR:
			return(org | (mod & ~SA_METHOD));
		case SA_TOGGLE:
			return(org ^ (mod & ~SA_METHOD));
		default:
			return(mod & ~SA_METHOD);
	}
}

#endif


