/*
 * Header Name:		terminal.h
 * Version:		0.1
 * Description:		Terminal Manager Interface
 */

#ifndef _WIN32_TERMINAL_H
#define _WIN32_TERMINAL_H

#include <windows.h>

struct terminal_s;
extern struct terminal_s *terminal;

int init_terminal(void);
int release_terminal(void);

struct terminal_s *terminal_create(struct terminal_s *, short, short, int);
int terminal_destroy(struct terminal_s *);
int terminal_print(struct terminal_s *, char *, int);
void terminal_clear(struct terminal_s *, short, short, short, short);
void terminal_move(struct terminal_s *, short, short);
int terminal_control(struct terminal_s *, int, ...);

int terminal_free(struct terminal_s *);
void terminal_refresh(struct terminal_s *);
int terminal_resizing(struct terminal_s *, RECT *, int);
int terminal_convert_char(int);
struct terminal_s *terminal_find(HWND);
HWND terminal_get_window(struct terminal_s *);
int terminal_get_number(void);

#endif


