/*
 * Header Name:		surface.h
 * Description:		Frontend Drawing Surface Header
 */

#ifndef _STUTTER_FRONTEND_SURFACE_H
#define _STUTTER_FRONTEND_SURFACE_H

#include <stutter/frontend/widget.h>

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

#define SCC_SET_ROOT		0x0011
#define SCC_SET_FOCUS		0x0012
#define SCC_REFRESH		0x0020
#define SCC_GET_ATTRIB		0x0030
#define SCC_SET_ATTRIB		0x0031
#define SCC_MODIFY_ATTRIB	0x0032
#define SCC_RESIZE		0x0040
#define SCC_MOVE_CURSOR		0x0050

#define SUR_BF_UNDESTROYABLE	0x0001

#define surface_get_width_m(surface)		( SURFACE_S(surface)->width )
#define surface_get_height_m(surface)		( SURFACE_S(surface)->height )

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

struct surface_s;
struct surface_type_s;

#define SURFACE_S(surface)	((struct surface_s *) (surface))
#define SURFACE_TYPE_S(type)	((struct surface_type_s *) (type))

#define surface_create_m(surface_type, parent, width, height, bitflags)	\
	( (SURFACE_TYPE_S(surface_type)->create)(parent, width, height, bitflags) )
#define surface_destroy_m(surface)					\
	( (SURFACE_S(surface)->type->destroy)(SURFACE_S(surface)) )
#define surface_print_m(surface, str, len)				\
	( (SURFACE_S(surface)->type->print)(SURFACE_S(surface), str, len) )
#define surface_clear_m(surface, x, y, width, height)			\
	( (SURFACE_S(surface)->type->clear)(SURFACE_S(surface), x, y, width, height) )
#define surface_move_m(surface, x, y)			\
	( (SURFACE_S(surface)->type->move)(SURFACE_S(surface), x, y) )
#define surface_control_m(surface, cmd, ...)				\
	( (SURFACE_S(surface)->type->control)(SURFACE_S(surface), cmd, __VA_ARGS__) )

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
	struct widget_s *root;
};

int init_surface(void);
int release_surface(void);

struct surface_s *create_surface(struct surface_type_s *, struct surface_s *, short, short, int);
int destroy_surface(struct surface_s *);
void refresh_surface(struct surface_s *);

struct surface_s *surface_get_current(void);
void surface_set_current(struct surface_s *);

struct surface_s *generate_surface(struct surface_type_s *, struct property_s *, struct layout_s *);

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


