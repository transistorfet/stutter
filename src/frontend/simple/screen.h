/*
 * Header Name:		screen.h
 * Version:		0.1
 * Description:		Screen Manager Interface
 */

#ifndef _SIMPLE_SCREEN_H
#define _SIMPLE_SCREEN_H

#include <lib/globals.h>

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

#define screen_width()			screen_last_width
#define screen_height()			screen_last_height

extern short screen_last_width, screen_last_height;

int init_screen(void);
int release_screen(void);

void screen_refresh(void);
void screen_clear(short, short, short, short);
void screen_move(short, short);
int screen_print(char *, int);
char screen_read_char(void);
void screen_set_attribs(int, int, int);

#endif


