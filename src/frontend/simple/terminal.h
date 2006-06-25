/*
 * Header Name:		terminal.h
 * Version:		0.1
 * Description:		Terminal Manager Interface
 */

#ifndef _SIMPLE_TERMINAL_H
#define _SIMPLE_TERMINAL_H

#include <stutter/lib/globals.h>

#define SC_NORMAL	0x00
#define SC_BOLD		0x01
#define SC_INVERSE	0x02
#define SC_BLINK	0x03

#define SC_BLACK	0x00
#define SC_RED		0x01
#define SC_GREEN	0x02
#define SC_YELLOW	0x03
#define SC_BLUE		0x04
#define SC_MAGENTA	0x05
#define SC_CYAN		0x06
#define SC_WHITE	0x07

#define terminal_width()			terminal_last_width
#define terminal_height()			terminal_last_height

extern short terminal_last_width, terminal_last_height;

int init_terminal(void);
int release_terminal(void);

void terminal_refresh(void);
void terminal_clear(short, short, short, short);
void terminal_move(short, short);
int terminal_print(char *, int);
int terminal_read_char(void);
void terminal_set_attribs(int, int, int);

#endif


