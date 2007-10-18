/*
 * Header Name:		terminal.h
 * Description:		Terminal Manager Interface
 */

#ifndef _CURSES_TERMINAL_H
#define _CURSES_TERMINAL_H

struct terminal_s;

int init_terminal(void);
int release_terminal(void);

struct terminal_s *terminal_create(struct terminal_s *, short, short, int);
int terminal_destroy(struct terminal_s *);
int terminal_print(struct terminal_s *, char *, int);
void terminal_clear(struct terminal_s *, short, short, short, short);
void terminal_move(struct terminal_s *, short, short);
int terminal_control(struct terminal_s *, int, ...);

#endif


