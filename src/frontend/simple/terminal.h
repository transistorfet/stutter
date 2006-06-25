/*
 * Header Name:		terminal.h
 * Version:		0.1
 * Description:		Terminal Manager Interface
 */

#ifndef _SIMPLE_TERMINAL_H
#define _SIMPLE_TERMINAL_H

#include <stutter/lib/globals.h>

#define TA_NORMAL	0x00
#define TA_BOLD		0x01
#define TA_INVERSE	0x02
#define TA_BLINK	0x03

#define TA_BLACK	0x00
#define TA_RED		0x01
#define TA_GREEN	0x02
#define TA_YELLOW	0x03
#define TA_BLUE		0x04
#define TA_MAGENTA	0x05
#define TA_CYAN		0x06
#define TA_WHITE	0x07

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


