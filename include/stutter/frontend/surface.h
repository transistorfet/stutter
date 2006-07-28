/*
 * Header Name:		surface.h
 * Version:		0.1
 * Description:		Frontend Drawing Surface Header
 */

#ifndef _STUTTER_FRONTEND_SURFACE_H
#define _STUTTER_FRONTEND_SURFACE_H

#define SCC_GET_ATTRIB		0x0010
#define SCC_SET_ATTRIB		0x0020

typedef struct attrib_s {
	int fg;
	int bg;
	int attrib;
} attrib_t;

typedef surface_s;

typedef struct surface_s *(*surface_create_t)(struct surface_s *surface, short width, short height, int bitflags);
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
	short y,
	short width;
	short height;
};

#endif


