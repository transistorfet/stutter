/*
 * Header Name:		surface.h
 * Description:		Frontend Drawing Surface Header
 */

#ifndef _STUTTER_FRONTEND_COMMON_SURFACE_H
#define _STUTTER_FRONTEND_COMMON_SURFACE_H

#include <stutter/object.h>
#include <stutter/frontend/common/layout.h>

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

#define SURFACE_GET_WIDTH(surface)		( FE_SURFACE(surface)->width )
#define SURFACE_GET_HEIGHT(surface)		( FE_SURFACE(surface)->height )

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

struct fe_widget;
struct fe_surface;
struct fe_surface_type;

#define FE_SURFACE(ptr)			( (struct fe_surface *) (ptr) )
#define FE_SURFACE_TYPE(ptr)		( (struct fe_surface_type *) (ptr) )
#define FE_SURFACE_GET_TYPE(ptr)	( FE_SURFACE_TYPE(OBJECT_S(ptr)->type) )

#define FE_SURFACE_PRINT(surface, str, len)				\
	( (FE_SURFACE_GET_TYPE(surface)->print)(FE_SURFACE(surface), str, len) )
#define FE_SURFACE_CLEAR(surface, x, y, width, height)			\
	( (FE_SURFACE_GET_TYPE(surface)->clear)(FE_SURFACE(surface), x, y, width, height) )
#define FE_SURFACE_MOVE(surface, x, y)			\
	( (FE_SURFACE_GET_TYPE(surface)->move)(FE_SURFACE(surface), x, y) )
#define FE_SURFACE_CONTROL(surface, cmd, ...)				\
	( (FE_SURFACE_GET_TYPE(surface)->control)(FE_SURFACE(surface), cmd, __VA_ARGS__) )

typedef int (*fe_surface_print_t)(struct fe_surface *surface, const char *str, int len);
typedef void (*fe_surface_clear_t)(struct fe_surface *surface, short x, short y, short width, short height);
typedef void (*fe_surface_move_t)(struct fe_surface *surface, short x, short y);
typedef int (*fe_surface_control_t)(struct fe_surface *surface, int cmd, ...);

struct fe_surface {
	struct object_s object;
	struct fe_surface *next;
	int bitflags;
	short x;
	short y;
	short width;
	short height;
	struct fe_widget *root;
};

struct fe_surface_type {
	struct object_type_s object_type;
	fe_surface_print_t print;
	fe_surface_clear_t clear;
	fe_surface_move_t move;
	fe_surface_control_t control;
};

extern struct fe_surface_type fe_surface_type;

int init_surface(void);
int release_surface(void);

int fe_surface_init(struct fe_surface *, const char *, va_list);
void fe_surface_release(struct fe_surface *);
void fe_surface_refresh(struct fe_surface *);

struct fe_surface *fe_surface_get_current(void);
void fe_surface_set_current(struct fe_surface *);

struct fe_surface *fe_surface_generate(struct fe_surface_type *, struct property_s *, struct fe_layout *);

/*** Miscellaneous Macros and Inline Functions ***/

static inline short fe_surface_modify_style(char method, short org, short mod)
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


