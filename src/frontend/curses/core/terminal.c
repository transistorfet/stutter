/*
 * Module Name:		terminal.c
 * System Requirements:	Curses Library
 * Description:		Curses Terminal Manager
 */

#include <curses.h>
#include <stdlib.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/memory.h>
#include <stutter/linear.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/frontend/keycodes.h>
#include <stutter/frontend/common/key.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/layout.h>
#include <stutter/frontend/common/colourmap.h>
#include <stutter/frontend/curses/interface.h>
#include <stutter/frontend/curses/terminal.h>

#define FE_TERMINAL_DEFAULT_WIDTH		80
#define FE_TERMINAL_DEFAULT_HEIGHT		25

static int terminal_initialized = 0;
static struct fe_interface *stdin_inter;
static struct fe_terminal *terminal_list = NULL;
static attrib_t terminal_def_attrib = { 0, SA_NORMAL, { SC_ENC_RGBA, SC_RGBA_WHITE }, { SC_ENC_RGBA, SC_RGBA_BLACK } };

struct fe_surface_type fe_terminal_type = { {
	OBJECT_TYPE_S(&fe_surface_type),
	"terminal",
	sizeof(struct fe_terminal),
	NULL,
	(object_init_t) fe_terminal_init,
	(object_release_t) fe_terminal_release },
	(fe_surface_print_t) fe_terminal_print,
	(fe_surface_clear_t) fe_terminal_clear,
	(fe_surface_move_t) fe_terminal_move,
	(fe_surface_control_t) fe_terminal_control
};

static int fe_terminal_check_input(void *, struct fe_interface *);
static void fe_terminal_set_attribs(struct fe_terminal *, attrib_t);
static inline void fe_terminal_refresh(struct fe_terminal *);
static inline int fe_terminal_convert_colour(colour_t);
static inline int fe_terminal_convert_char(int);

int init_terminal(void)
{
	int i, j;

	if (terminal_initialized)
		return(0);
	if (init_colourmap())
		return(-1);
	layout_register_type("terminal", LAYOUT_RT_SURFACE, (fe_layout_create_t) fe_surface_generate, &fe_terminal_type);

	/** Create an interface that calls us on a keyboard event */
	if (!(stdin_inter = FE_INTERFACE(create_object(OBJECT_TYPE_S(&fe_interface_type), ""))))
		return(-1);
	stdin_inter->read = 0;
	fe_interface_set_callback(stdin_inter, IO_COND_READ, (callback_t) fe_terminal_check_input, NULL);

	/** Initialize ncurses */
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

	// TODO Should this totally be changed?
	//if ((type = object_find_type("attrib:fe", NULL))) {
	//	add_variable(NULL, type, "fe.fg", 0, "pointer", &terminal_def_attrib.fg);
	//	add_variable(NULL, type, "fe.bg", 0, "pointer", &terminal_def_attrib.bg);
	//}
	terminal_initialized = 1;
	return(0);
}

int release_terminal(void)
{
	if (!terminal_initialized)
		return(0);
	// TODO you don't destroy the terminal list here (but beware of the gremlin)
	layout_unregister_type("terminal");
	destroy_object(OBJECT_S(stdin_inter));
	endwin();
	release_layout();
	release_colourmap();
	terminal_initialized = 0;
	return(0);
}

int fe_terminal_init(struct fe_terminal *terminal, const char *params, va_list va)
{
	int screen_width, screen_height;
	int width = FE_TERMINAL_DEFAULT_WIDTH, height = FE_TERMINAL_DEFAULT_HEIGHT;

	if (fe_surface_init(FE_SURFACE(terminal), params, va))
		return(-1);
	FE_SURFACE(terminal)->bitflags = 0;
	FE_SURFACE(terminal)->x = 0;
	FE_SURFACE(terminal)->y = 0;
	getmaxyx(stdscr, screen_height, screen_width);
	FE_SURFACE(terminal)->width = ((width == -1) || (width >= screen_width)) ? screen_width : width;
	FE_SURFACE(terminal)->height = ((height == -1) || (height >= screen_height)) ? screen_height : height;
	FE_SURFACE(terminal)->root = NULL;

	terminal->next = terminal_list;
	terminal_list = terminal;
	fe_terminal_control(terminal, SCC_MODIFY_ATTRIB, SA_METHOD_SET, SA_NORMAL, SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR, SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR);
	return(0);
}

void fe_terminal_release(struct fe_terminal *terminal)
{
	struct fe_terminal *cur, *prev;

	for (cur = terminal_list, prev = NULL; cur; prev = cur, cur = cur->next) {
		if (cur == terminal) {
			if (prev)
				prev->next = cur->next;
			else
				terminal_list = cur->next;
			break;
		}
	}
	if (FE_SURFACE(terminal)->root)
		destroy_object(OBJECT_S(FE_SURFACE(terminal)->root));
	fe_surface_release(FE_SURFACE(terminal));
}

int fe_terminal_print(struct fe_terminal *terminal, const char *str, int length)
{
	int i;

	move(FE_SURFACE(terminal)->y, FE_SURFACE(terminal)->x);
	fe_terminal_set_attribs(terminal, terminal->attrib);
	if (length == -1)
		length = strlen(str);
	for (i = 0;(str[i] != '\0') && (i < length);i++)
		addch(str[i]);
	FE_SURFACE(terminal)->x += (length % FE_SURFACE(terminal)->width);
	FE_SURFACE(terminal)->y += (length / FE_SURFACE(terminal)->width);
	if (FE_SURFACE(terminal)->y > FE_SURFACE(terminal)->height)
		FE_SURFACE(terminal)->y = FE_SURFACE(terminal)->y % FE_SURFACE(terminal)->height;
	return(0);
}

void fe_terminal_clear(struct fe_terminal *terminal, short x, short y, short width, short height)
{
	int i;

	fe_terminal_set_attribs(terminal, terminal->attrib);
	if (!width)
		width = FE_SURFACE(terminal)->width - x;
	if (!height)
		height = FE_SURFACE(terminal)->height - y;

	for (i = 0;i < height;i++) {
		move(y, x);
		hline(' ', width);
		y++;
	}
}

void fe_terminal_move(struct fe_terminal *terminal, short x, short y)
{
	FE_SURFACE(terminal)->x = (x < FE_SURFACE(terminal)->width) ? x : FE_SURFACE(terminal)->width - 1;
	FE_SURFACE(terminal)->y = (y < FE_SURFACE(terminal)->height) ? y : FE_SURFACE(terminal)->height - 1;
	move(FE_SURFACE(terminal)->y, FE_SURFACE(terminal)->x);
}

int fe_terminal_control(struct fe_terminal *terminal, int cmd, ...)
{
	va_list va;

	va_start(va, cmd);
	switch (cmd) {
		case SCC_SET_ROOT: {
			struct fe_widget *root, **widget_ptr;

			root = va_arg(va, struct fe_widget *);
			widget_ptr = va_arg(va, struct fe_widget **);
			if (widget_ptr)
				*widget_ptr = FE_SURFACE(terminal)->root;
			else if (FE_SURFACE(terminal)->root)
				destroy_object(OBJECT_S(FE_SURFACE(terminal)->root));
			if ((FE_SURFACE(terminal)->root = root))
				fe_widget_control(FE_SURFACE(terminal)->root, WCC_SET_WINDOW, 0, 0, FE_SURFACE(terminal)->width, FE_SURFACE(terminal)->height);
			return(0);
		}
		case SCC_REFRESH: {
			fe_terminal_refresh(terminal);
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
			terminal->attrib.style = fe_surface_modify_style(attrib->method, terminal->attrib.style, attrib->style);
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
			terminal->attrib.style = fe_surface_modify_style(method, terminal->attrib.style, arg);
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
			x = (x < FE_SURFACE(terminal)->width) ? ( (x >= 0) ? x : 0 ) : FE_SURFACE(terminal)->width - 1;
			y = (y < FE_SURFACE(terminal)->height) ? ( (y >= 0) ? y : 0 ) : FE_SURFACE(terminal)->height - 1;
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
static int fe_terminal_check_input(void *ptr, struct fe_interface *inter)
{
	int ch;
	struct fe_widget *focus;
	struct fe_surface *surface;

	if (!(ch = fe_terminal_convert_char(getch())))
		return(-1);
	/** If the keybinding system handles the key then just return */
	if (!fe_key_process(ch))
		return(0);
	/** Otherwise see if the widget with focus can process it */
	if (!(surface = fe_surface_get_current()) || !surface->root
	    || fe_widget_control(surface->root, WCC_GET_FOCUS, &focus))
		return(-1);
	fe_widget_control(focus, WCC_PROCESS_CHAR, ch);
	return(0);
}

/**
 * Set the current attributes of the given terminal to the given attributes.
 */
static void fe_terminal_set_attribs(struct fe_terminal *terminal, attrib_t attrib)
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
		colour = COLOR_PAIR((fe_terminal_convert_colour(attrib.fg) * 8) + fe_terminal_convert_colour(attrib.bg));
	else
		colour = COLOR_PAIR((fe_terminal_convert_colour(attrib.bg) * 8) + fe_terminal_convert_colour(attrib.fg));
	attrset(attr | colour);
}

/**
 * Redraw and refresh the terminal by calling all registered refresh callback
 * functions and return 0 on success.
 */
static inline void fe_terminal_refresh(struct fe_terminal *terminal)
{
	widget_size_t size;

	getmaxyx(stdscr, FE_SURFACE(terminal)->height, FE_SURFACE(terminal)->width);
	if (FE_SURFACE(terminal)->root) {
		fe_widget_control(FE_SURFACE(terminal)->root, WCC_GET_WINDOW, NULL, &size);
		if ((FE_SURFACE(terminal)->width != size.width) || (FE_SURFACE(terminal)->height != size.height))
			fe_widget_control(FE_SURFACE(terminal)->root, WCC_SET_WINDOW, 0, 0, FE_SURFACE(terminal)->width, FE_SURFACE(terminal)->height);
		FE_WIDGET_REFRESH(FE_SURFACE(terminal)->root, FE_SURFACE(terminal));
	}
	refresh();
}

/**
 * Convert the given colour and encoding into an 8-colour terminal colour number.
 */
static inline int fe_terminal_convert_colour(colour_t colour)
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
static inline int fe_terminal_convert_char(int ch)
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


