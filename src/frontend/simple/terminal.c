/*
 * Module Name:		terminal.c
 * Version:		0.1
 * Module Requirements:	(none)
 * System Requirements:	Curses Library
 * Description:		Curses Terminal Manager
 */

#include <curses.h>
#include <stdlib.h>

#include "terminal.h"

short terminal_last_width, terminal_last_height;
static int terminal_attribs[] = { A_NORMAL, A_BOLD, A_REVERSE, A_BLINK };

/**
 * Initialize the terminal and return 0 on success.
 */
int init_terminal(void)
{
	int i, j;

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
	getmaxyx(stdscr, terminal_last_height, terminal_last_width);
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

/**
 * Redraw and refresh the terminal by calling all registered refresh callback
 * functions and return 0 on success.
 */
void terminal_refresh(void)
{
	getmaxyx(stdscr, terminal_last_height, terminal_last_width);
	refresh();
}

/**
 * Fill an area of the terminal of the given size and starting at the given
 * x,y coordinates with a blank character.
 */
void terminal_clear(short x, short y, short width, short height)
{
	int i;

	if (!width)
		width = terminal_last_width - x;
	if (!height)
		height = terminal_last_height - y;

	for (i = 0;i < height;i++) {
		move(y, x);
		hline(' ', width);
		y++;
	}
}

/**
 * Move the cursor of the given terminal to the given x, y coordinates.
 */
void terminal_move(short x, short y)
{
	move(y, x);
}

/**
 * Print the given string to the given length (or the whole string if
 * the given length is 0) to the given terminal and return 0 on success.
 */
int terminal_print(char *str, int length)
{
	int i;
	int attrib = SC_NORMAL;

	if (!length)
		length = 2147483647;
	for (i = 0;(str[i] != '\0') && (i < length);i++) {
		if (str[i] == 0x02) {
			attrib = (attrib & ~SC_BOLD) | (SC_BOLD & (attrib ^ SC_BOLD));
			terminal_set_attribs(attrib, 0, 0);
		}
		else
			addch(str[i]);
	}
	return(0);
}

int terminal_read_char(void)
{
	int ch;

	ch = getch();
	return(ch);
}

/**
 * Set the current attributes of the given terminal to the given attributes.
 */
void terminal_set_attribs(int attrib, int fg, int bg)
{
	attrset(terminal_attribs[attrib] | COLOR_PAIR((bg * 8) + fg));
}



