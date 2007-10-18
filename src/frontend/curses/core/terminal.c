/*
 * Module Name:		terminal.c
 * System Requirements:	Curses Library
 * Description:		Curses Terminal Manager
 */

#include <curses.h>
#include <stdlib.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/key.h>
#include <stutter/type.h>
#include <stutter/memory.h>
#include <stutter/linear.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/keycodes.h>
#include <stutter/frontend/common/layout.h>
#include <stutter/frontend/common/colourmap.h>
#include "desc.h"
#include "terminal.h"

struct terminal_s {
	struct surface_s surface;
	attrib_t attrib;
	linear_node_v(terminal_s) list;
};

static int terminal_initialized = 0;
static struct fe_descriptor_s *stdin_desc;
static struct fe_descriptor_list_s *desc_list;
static linear_list_v(terminal_s) terminal_list;
static attrib_t terminal_def_attrib = { 0, SA_NORMAL, { SC_ENC_RGBA, SC_RGBA_WHITE }, { SC_ENC_RGBA, SC_RGBA_BLACK } };

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

static int terminal_check_input(void *, struct fe_descriptor_s *);
static void terminal_set_attribs(struct terminal_s *, attrib_t);
static inline void terminal_refresh(struct terminal_s *);
static inline int terminal_convert_colour(colour_t);
static inline int terminal_convert_char(int);

int init_terminal(void)
{
	int i, j;
	struct type_s *type;

	if (terminal_initialized)
		return(0);
	if (init_colourmap())
		return(-1);
	layout_register_type("terminal", LAYOUT_RT_SURFACE, (layout_create_t) generate_surface, &terminal_type);

	if (!(desc_list = fe_desc_create_list((destroy_t) NULL)))
		return(-1);
	if (!(stdin_desc = fe_desc_create(desc_list, 0)))
		return(-1);
	stdin_desc->read = 0;
	fe_desc_set_callback(stdin_desc, IO_COND_READ, (callback_t) terminal_check_input, NULL);

	initscr();
	if (has_colors()) {
		start_color();
		for (i = 0;i < 8;i++) {
			for (j = 0;j < 8;j++) {
				init_pair(i * 8 + j, j, i);
			}
		}
	}
	raw();
	keypad(stdscr, TRUE);
	noecho();
	scrollok(stdscr, 1);
	nodelay(stdscr, TRUE);

	if ((type = find_type("attrib:fe"))) {
		add_variable(NULL, type, "fe.fg", 0, "pointer", &terminal_def_attrib.fg);
		add_variable(NULL, type, "fe.bg", 0, "pointer", &terminal_def_attrib.bg);
	}
	terminal_initialized = 1;
	return(0);
}

int release_terminal(void)
{
	if (!terminal_initialized)
		return(0);
	// TODO you don't destroy the terminal list here (but beware of the gremlin)
	layout_unregister_type("terminal");
	fe_desc_destroy_list(desc_list);
	endwin();
	release_layout();
	release_colourmap();
	terminal_initialized = 0;
	return(0);
}

struct terminal_s *terminal_create(struct terminal_s *parent, short width, short height, int bitflags)
{
	struct terminal_s *terminal;
	int screen_width, screen_height;

	if (linear_first_v(terminal_list))
		return(NULL);
	if (!(terminal = memory_alloc(sizeof(struct terminal_s))))
		return(NULL);
	SURFACE_S(terminal)->type = &terminal_type;
	SURFACE_S(terminal)->bitflags = bitflags;
	SURFACE_S(terminal)->x = 0;
	SURFACE_S(terminal)->y = 0;
	getmaxyx(stdscr, screen_height, screen_width);
	SURFACE_S(terminal)->width = ((width == -1) || (width >= screen_width)) ? screen_width : width;
	SURFACE_S(terminal)->height = ((height == -1) || (height >= screen_height)) ? screen_height : height;
	SURFACE_S(terminal)->root = NULL;

	linear_add_node_v(terminal_list, list, terminal);
	terminal_control(terminal, SCC_MODIFY_ATTRIB, SA_METHOD_SET, SA_NORMAL, SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR, SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR);

	return(terminal);
}

int terminal_destroy(struct terminal_s *terminal)
{
	linear_remove_node_v(terminal_list, list, terminal);
	if (SURFACE_S(terminal)->root)
		destroy_widget(WIDGET_S(SURFACE_S(terminal)->root));
	memory_free(terminal);
	return(0);
}

int terminal_print(struct terminal_s *terminal, char *str, int length)
{
	int i;

	move(SURFACE_S(terminal)->y, SURFACE_S(terminal)->x);
	terminal_set_attribs(terminal, terminal->attrib);
	if (length == -1)
		length = strlen(str);
	for (i = 0;(str[i] != '\0') && (i < length);i++)
		addch(str[i]);
	SURFACE_S(terminal)->x += (length % SURFACE_S(terminal)->width);
	SURFACE_S(terminal)->y += (length / SURFACE_S(terminal)->width);
	if (SURFACE_S(terminal)->y > SURFACE_S(terminal)->height)
		SURFACE_S(terminal)->y = SURFACE_S(terminal)->y % SURFACE_S(terminal)->height;
	return(0);
}

void terminal_clear(struct terminal_s *terminal, short x, short y, short width, short height)
{
	int i;

	terminal_set_attribs(terminal, terminal->attrib);
	if (!width)
		width = SURFACE_S(terminal)->width - x;
	if (!height)
		height = SURFACE_S(terminal)->height - y;

	for (i = 0;i < height;i++) {
		move(y, x);
		hline(' ', width);
		y++;
	}
}

void terminal_move(struct terminal_s *terminal, short x, short y)
{
	SURFACE_S(terminal)->x = (x < SURFACE_S(terminal)->width) ? x : SURFACE_S(terminal)->width - 1;
	SURFACE_S(terminal)->y = (y < SURFACE_S(terminal)->height) ? y : SURFACE_S(terminal)->height - 1;
	move(SURFACE_S(terminal)->y, SURFACE_S(terminal)->x);
}

int terminal_control(struct terminal_s *terminal, int cmd, ...)
{
	va_list va;

	va_start(va, cmd);
	switch (cmd) {
		case SCC_SET_ROOT: {
			struct widget_s *root, **widget_ptr;

			root = va_arg(va, struct widget_s *);
			widget_ptr = va_arg(va, struct widget_s **);
			if (widget_ptr)
				*widget_ptr = SURFACE_S(terminal)->root;
			else if (SURFACE_S(terminal)->root)
				destroy_widget(SURFACE_S(terminal)->root);
			if ((SURFACE_S(terminal)->root = root)) {
				widget_control(SURFACE_S(terminal)->root, WCC_SET_SURFACE, terminal);
				widget_control(SURFACE_S(terminal)->root, WCC_SET_WINDOW, 0, 0, SURFACE_S(terminal)->width, SURFACE_S(terminal)->height);
			}
			return(0);
		}
		case SCC_REFRESH: {
			terminal_refresh(terminal);
			return(0);
		}
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
			terminal->attrib.style = surface_modify_style(attrib->method, terminal->attrib.style, attrib->style);
			if ((attrib->fg.enc != SC_ENC_MAPPING) || (attrib->fg.colour != SC_MAP_CURRENT_COLOUR))
				terminal->attrib.fg = attrib->fg;
			if ((attrib->bg.enc != SC_ENC_MAPPING) || (attrib->bg.colour != SC_MAP_CURRENT_COLOUR))
				terminal->attrib.bg = attrib->bg;
			return(0);
		}
		case SCC_MODIFY_ATTRIB: {
			int method, enc;
			int arg;

			method = va_arg(va, int);
			arg = va_arg(va, int);
			terminal->attrib.method = 0;
			terminal->attrib.style = surface_modify_style(method, terminal->attrib.style, arg);
			enc = va_arg(va, int);
			arg = va_arg(va, int);
			if ((enc != SC_ENC_MAPPING) || (arg != SC_MAP_CURRENT_COLOUR)) {
				terminal->attrib.fg.enc = enc;
				terminal->attrib.fg.colour = arg;
			}
			enc = va_arg(va, int);
			arg = va_arg(va, int);
			if ((enc != SC_ENC_MAPPING) || (arg != SC_MAP_CURRENT_COLOUR)) {
				terminal->attrib.bg.enc = enc;
				terminal->attrib.bg.colour = arg;
			}
			return(0);
		}
		case SCC_MOVE_CURSOR: {
			int x, y;
			x = va_arg(va, int);
			y = va_arg(va, int);
			x = (x < SURFACE_S(terminal)->width) ? ( (x >= 0) ? x : 0 ) : SURFACE_S(terminal)->width - 1;
			y = (y < SURFACE_S(terminal)->height) ? ( (y >= 0) ? y : 0 ) : SURFACE_S(terminal)->height - 1;
			move(y, x);
			return(0);
		}
		default:
			break;
	}
	va_end(va);
	return(-1);
}

/*** Local Functions ***/

/**
 * Check for keypresses.
 */
static int terminal_check_input(void *ptr, struct fe_descriptor_s *desc)
{
	int ch;
	struct widget_s *focus;

	// TODO modify to find the corresponding terminal somehow and use that as ref
	if ((ch = terminal_convert_char(getch())) && process_key(ch) && (focus = (struct widget_s *) fe_get_focus(NULL)))
		widget_control(focus, WCC_PROCESS_CHAR, ch);
	return(0);
}

/**
 * Set the current attributes of the given terminal to the given attributes.
 */
static void terminal_set_attribs(struct terminal_s *terminal, attrib_t attrib)
{
	int colour;
	int attr = A_NORMAL;

	if ((attrib.fg.enc == SC_ENC_MAPPING) && (attrib.fg.colour == SC_MAP_DEFAULT_COLOUR))
		attrib.fg = terminal_def_attrib.fg;
	if ((attrib.bg.enc == SC_ENC_MAPPING) && (attrib.bg.colour == SC_MAP_DEFAULT_COLOUR))
		attrib.bg = terminal_def_attrib.bg;

	if (attrib.style & SA_BOLD)
		attr = A_BOLD;

	if (attrib.style & SA_INVERSE)
		colour = COLOR_PAIR((terminal_convert_colour(attrib.fg) * 8) + terminal_convert_colour(attrib.bg));
	else
		colour = COLOR_PAIR((terminal_convert_colour(attrib.bg) * 8) + terminal_convert_colour(attrib.fg));
	attrset(attr | colour);
}

/**
 * Redraw and refresh the terminal by calling all registered refresh callback
 * functions and return 0 on success.
 */
static inline void terminal_refresh(struct terminal_s *terminal)
{
	widget_size_t size;

	getmaxyx(stdscr, SURFACE_S(terminal)->height, SURFACE_S(terminal)->width);
	if (SURFACE_S(terminal)->root) {
		widget_control(SURFACE_S(terminal)->root, WCC_GET_WINDOW, NULL, &size);
		if ((SURFACE_S(terminal)->width != size.width) || (SURFACE_S(terminal)->height != size.height))
			widget_control(SURFACE_S(terminal)->root, WCC_SET_WINDOW, 0, 0, SURFACE_S(terminal)->width, SURFACE_S(terminal)->height);
		widget_refresh_m(SURFACE_S(terminal)->root);
	}
	refresh();
}

/**
 * Convert the given colour and encoding into an 8-colour terminal colour number.
 */
static inline int terminal_convert_colour(colour_t colour)
{
	int index = 0;

	if (colour.enc == SC_ENC_MAPPING)
		colour.colour = colourmap_get_colour(NULL, SC_ENC_RGBA, colour.colour);
	else if (colour.enc != SC_ENC_RGBA)
		return(0);

	if ((colour.colour & 0x000000ff) >= 0x7F)
		index |= 0x04;
	if ((colour.colour & 0x0000ff00) >= 0x7F00)
		index |= 0x02;
	if ((colour.colour & 0x00ff0000) >= 0x7F0000)
		index |= 0x01;
	return(index);
}

/**
 * Read a character and convert it to keycodes.h characters
 */
static inline int terminal_convert_char(int ch)
{
	switch (ch) {
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


