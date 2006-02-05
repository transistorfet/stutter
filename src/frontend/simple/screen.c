/*
 * Module Name:		screen.c
 * Version:		0.1
 * Module Requirements:	(none)
 * System Requirements:	Curses Library
 * Description:		Curses Screen Manager
 */

#include <curses.h>
#include <stdlib.h>

#include "screen.h"

short screen_last_width, screen_last_height;
static int screen_attribs[] = { A_NORMAL, A_BOLD, A_REVERSE, A_BLINK };

/**
 * Initialize the screen and return 0 on success.
 */
int init_screen(void)
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
	getmaxyx(stdscr, screen_last_height, screen_last_width);
	return(0);
}

/**
 * Release all screen resources and return 0 on success.
 */
int release_screen(void)
{
	endwin();
	return(0);
}

/**
 * Redraw and refresh the screen by calling all registered refresh callback
 * functions and return 0 on success.
 */
void screen_refresh(void)
{
	getmaxyx(stdscr, screen_last_height, screen_last_width);
	refresh();
}

/**
 * Fill an area of the screen of the given size and starting at the given
 * x,y coordinates with a blank character.
 */
void screen_clear(short x, short y, short width, short height)
{
	int i;

	if (!width)
		width = screen_last_width - x;
	if (!height)
		height = screen_last_height - y;

	for (i = 0;i < height;i++) {
		move(y, x);
		hline(' ', width);
		y++;
	}
}

/**
 * Move the cursor of the given screen to the given x, y coordinates.
 */
void screen_move(short x, short y)
{
	move(y, x);
}

/**
 * Print the given string to the given length (or the whole string if
 * the given length is 0) to the given screen and return 0 on success.
 */
int screen_print(char *str, int length)
{
	int i;

	if (!length)
		length = 2147483647;
	for (i = 0;(str[i] != '\0') && (i < length);i++)
		addch(str[i]);
	return(0);
}

char screen_read_char(void)
{
	int ch;

	ch = getch();
//	switch (ch) {
//		case 0x7f:
//		case KEY_BACKSPACE:
//			return(0x08);
//		case KEY_UP:
//			return(KC_UP);
//		case KEY_DOWN:
//			return(KC_DOWN);
//		case KEY_RIGHT:
//			return(KC_RIGHT);
//		case KEY_LEFT:
//			return(KC_LEFT);
//		case KEY_HOME:
//			return(KC_HOME);
//		case KEY_END:
//			return(KC_END);
//		case KEY_NPAGE:
//			return(KC_PAGEUP);
//		case KEY_PPAGE:
//			return(KC_PAGEDOWN);
//		case KEY_ENTER:
//			return('\n');
//		default:
			return(ch);
//	}
}

/**
 * Set the current attributes of the given screen to the given attributes.
 */
void screen_set_attribs(int attrib, int fg, int bg)
{
	attrset(screen_attribs[attrib] | COLOR_PAIR((bg * 8) + fg));
}



