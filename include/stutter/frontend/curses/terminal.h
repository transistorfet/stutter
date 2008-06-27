/*
 * Header Name:		terminal.h
 * Description:		Terminal Manager Interface
 */

#ifndef _STUTTER_FRONTEND_CURSES_TERMINAL_H
#define _STUTTER_FRONTEND_CURSES_TERMINAL_H

#include <stdarg.h>

#include <stutter/object.h>
#include <stutter/frontend/common/surface.h>

#define FE_TERMINAL(ptr)		( (struct fe_terminal *) (ptr) )

struct fe_terminal {
	struct fe_surface surface;
	attrib_t attrib;
	struct fe_terminal *next;
};

extern struct fe_surface_type fe_terminal_type;

int init_terminal(void);
int release_terminal(void);

int fe_terminal_init(struct fe_terminal *, const char *, va_list);
void fe_terminal_release(struct fe_terminal *);
int fe_terminal_print(struct fe_terminal *, const char *, int);
void fe_terminal_clear(struct fe_terminal *, short, short, short, short);
void fe_terminal_move(struct fe_terminal *, short, short);
int fe_terminal_control(struct fe_terminal *, int, ...);

#endif


