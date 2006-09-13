/*
 * Module Name:		terminal.c
 * Version:		0.1
 * Module Requirements:	(none)
 * System Requirements:	Curses Library
 * Description:		Curses Terminal Manager
 */

#include <curses.h>
#include <stdlib.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/lib/memory.h>
#include <stutter/lib/linear.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/keycodes.h>
#include "terminal.h"

struct terminal_s {
	struct surface_s surface;
	attrib_t attrib;
	linear_node_v(terminal_s) list;
};

struct terminal_s *terminal;

static linear_list_v(terminal_s) terminal_list;
static struct terminal_s *terminal_current = NULL;

struct surface_type_s terminal_type = {
	"curses-terminal",
	0,
	(surface_create_t) terminal_create,
	(surface_destroy_t) terminal_destroy,
	(surface_print_t) terminal_print,
	(surface_clear_t) terminal_clear,
	(surface_move_t) terminal_move,
	(surface_control_t) terminal_control
};

static void terminal_set_attribs(attrib_t);
static inline int terminal_convert_colour(int);

/**
 * Initialize the terminal and return 0 on success.
 */
int init_terminal(void)
{
	int i, j;

	if (terminal)
		return(1);
	initscr();
	if (has_colors()) {
		start_color();
		for (i = 0;i < 8;i++) {
			for (j = 0;j < 8;j++) {
				init_pair(i * 8 + j, j, i);
			}
		}
	}
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	scrollok(stdscr, 1);
	nodelay(stdscr, TRUE);

	if (!(terminal = terminal_create(NULL, -1, -1, 0)))
		return(-1);
	return(0);
}

/**
 * Release all terminal resources and return 0 on success.
 */
int release_terminal(void)
{
	endwin();
	return(0);
}

struct terminal_s *terminal_create(struct terminal_s *parent, short width, short height, int bitflags)
{
	struct terminal_s *terminal;
	int screen_width, screen_height;

	if (!(terminal = memory_alloc(sizeof(struct terminal_s))))
		return(NULL);
	terminal->surface.type = &terminal_type;
	terminal->surface.bitflags = bitflags;
	terminal->surface.x = 0;
	terminal->surface.y = 0;
	getmaxyx(stdscr, screen_height, screen_width);
	terminal->surface.width = ((width == -1) || (width >= screen_width)) ? screen_width : width;
	terminal->surface.height = ((height == -1) || (height >= screen_height)) ? screen_height : height;
	linear_add_node_v(terminal_list, list, terminal);
	terminal_control(terminal, SCC_MODIFY_ATTRIB, SA_NORMAL, SA_WHITE, SA_BLACK);

	return(terminal);
}

int terminal_destroy(struct terminal_s *terminal)
{
	linear_remove_node_v(terminal_list, list, terminal);
	memory_free(terminal);
	return(0);
}

int terminal_print(struct terminal_s *terminal, char *str, int length)
{
	int i;

	move(terminal->surface.y, terminal->surface.x);
	terminal_set_attribs(terminal->attrib);
	if (length == -1)
		length = strlen(str);
	for (i = 0;(str[i] != '\0') && (i < length);i++)
		addch(str[i]);
	terminal->surface.x += (length % terminal->surface.width);
	terminal->surface.y += (length / terminal->surface.width);
	if (terminal->surface.y > terminal->surface.height)
		terminal->surface.y = terminal->surface.y % terminal->surface.height;
	return(0);
}

void terminal_clear(struct terminal_s *terminal, short x, short y, short width, short height)
{
	int i;

	terminal_set_attribs(terminal->attrib);
	if (!width)
		width = terminal->surface.width - x;
	if (!height)
		height = terminal->surface.height - y;

	for (i = 0;i < height;i++) {
		move(y, x);
		hline(' ', width);
		y++;
	}
}

void terminal_move(struct terminal_s *terminal, short x, short y)
{
	terminal->surface.x = (x < terminal->surface.width) ? x : terminal->surface.width - 1;
	terminal->surface.y = (y < terminal->surface.height) ? y : terminal->surface.height - 1;
	move(terminal->surface.y, terminal->surface.x);
}

int terminal_control(struct terminal_s *terminal, int cmd, ...)
{
	va_list va;

	va_start(va, cmd);
	switch (cmd) {
		case SCC_GET_ATTRIB: {
			attrib_t *attrib;

			attrib = va_arg(va, attrib_t *);
			*attrib = terminal->attrib;
			return(0);
		}
		case SCC_SET_ATTRIB: {
			attrib_t *attrib;

			attrib = va_arg(va, attrib_t *);
			if (!attrib)
				return(-1);
			terminal->attrib.attrib = surface_modify_attrib(terminal->attrib.attrib, attrib->attrib);
			if (attrib->fg != -1)
				terminal->attrib.fg = attrib->fg;
			if (attrib->bg != -1)
				terminal->attrib.bg = attrib->bg;
			return(0);
		}
		case SCC_MODIFY_ATTRIB: {
			int arg;

			arg = va_arg(va, int);
			terminal->attrib.attrib = surface_modify_attrib(terminal->attrib.attrib, arg);
			arg = va_arg(va, int);
			if (arg != -1)
				terminal->attrib.fg = arg;
			arg = va_arg(va, int);
			if (arg != -1)
				terminal->attrib.bg = arg;
			return(0);
		}
		default:
			break;
	}
	va_end(va);
	return(-1);
}

/**
 * Redraw and refresh the terminal by calling all registered refresh callback
 * functions and return 0 on success.
 */
void terminal_refresh(struct terminal_s *terminal)
{
	getmaxyx(stdscr, terminal->surface.height, terminal->surface.width);
	refresh();
}

/**
 * Read a character and convert it to keycodes.h characters
 */
int terminal_read_char(void)
{
	int ch;

	switch (ch = getch()) {
		case 0x7f:
		case KEY_BACKSPACE:
			return(KC_BACKSPACE);
		case KEY_UP:
			return(KC_UP);
		case KEY_DOWN:
			return(KC_DOWN);
		case KEY_LEFT:
			return(KC_LEFT);
		case KEY_RIGHT:
			return(KC_RIGHT);
		default:
			return(ch);
	}
}

/*** Local Functions ***/

/**
 * Set the current attributes of the given terminal to the given attributes.
 */
static void terminal_set_attribs(attrib_t attrib)
{
	int colour;
	int attr = A_NORMAL;

	if (attrib.attrib & SA_BOLD)
		attr = A_BOLD;

	if (attrib.attrib & SA_INVERSE)
		colour = COLOR_PAIR((terminal_convert_colour(attrib.fg) * 8) + terminal_convert_colour(attrib.bg));
	else
		colour = COLOR_PAIR((terminal_convert_colour(attrib.bg) * 8) + terminal_convert_colour(attrib.fg));
	attrset(attr | colour);
}

/**
 * Convert the given RGB colour into an 8-colour terminal colour number.
 */
static inline int terminal_convert_colour(int colour)
{
	int index = 0;

	if ((colour & 0x000000ff) >= 0x80)
		index |= 0x04;
	if ((colour & 0x0000ff00) >= 0x8000)
		index |= 0x02;
	if ((colour & 0x00ff0000) >= 0x800000)
		index |= 0x01;
	return(index);
}


